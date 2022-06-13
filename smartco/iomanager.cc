#include "iomanager.h"



namespace smartco{

// 用于唤醒一个线程
void IOmanager::readAll(){
    uint8_t dummy[256];
    while(read(filedes[0], dummy, sizeof(dummy)) > 0);      
}

IOmanager::IOmanager(int thread_nums, bool use_main_thread, const std::string& name ):
           Scheduler(thread_nums, use_main_thread, name){
    // 初始化epoll
    epfd = epoll_create(INT8_MAX);
    event_ctx_list.resize(10);
    // 设置管道，用于唤醒当前主协程，防止锁定线程执行的协程遇见线程被阻塞很久到epoll_wait上
    pipe(filedes);
    SetNonblocking(filedes[0]);
    SetNonblocking(filedes[1]);
    addEvent(filedes[0], IOmanager::Event::READ, std::bind(&IOmanager::readAll, this));
}

IOmanager::~IOmanager(){
    close(epfd);
}

bool IOmanager::addEvent(int fd, IOmanager::Event event, std::function<void()> cb){
    event_ctx* ctx = nullptr;
    // 从event_ctx_list中访问信息，所以要上读锁
    ReadMutexLock<RWMutex> mutex(&rwmutex);
    if(fd < (int)event_ctx_list.size()){
        ctx = event_ctx_list[fd];
        mutex.unlock();
    }else{
        mutex.unlock();
        // 修改event_ctx_list信息，所以要上写锁
        WriteMutexLock<RWMutex> mutex2(&rwmutex);
        event_ctx_list.resize(event_ctx_list.size()*1.5);
    }

    // 对队列中的一个事件进行操作，上局部锁
    MutexLock<Mutex> mutex2(&ctx->m_mutex);
    // 判断当前事件是否在epoll上注册了，我们默认当ctx->events为NONE的时候，会将事件从epoll中删除
    int op = ctx->events?EPOLL_CTL_MOD:EPOLL_CTL_ADD;
    epoll_event ep_event;
    ep_event.data.fd = fd;
    ep_event.data.ptr = ctx;
    // EPOLLET不会存在于上下文的ctx->events中
    ep_event.events = EPOLLET | event | ctx->events;

    ctx->events = (Event)(ctx->events | event);

    int rt2 = epoll_ctl(epfd, op, fd, &ep_event);
    if (rt2)
    {
        return false;
        //错误处理
    }

    //填充读写事件
    task* m_task = ctx->getCtx(event);
    //当有参数传入的时候事件结果就相当于新建协程去执行cb，当cb为nullptr的时候，事件结果为唤醒该协程
    if(cb){
        m_task->cb.swap(cb);
    }else{
        m_task->cb = nullptr;
        m_task->m_fiber = Fiber::get_cur_fiber();
    }
    return true;


}

bool IOmanager::delEvent(int fd, IOmanager::Event event){
    // 读队列大小要加锁
    ReadMutexLock<RWMutex> mutex(&rwmutex);
    if(fd >= (int)event_ctx_list.size()){
        return false;
    }
    event_ctx* ctx = event_ctx_list[fd];
    mutex.unlock();

    MutexLock<Mutex> mutex2(&ctx->m_mutex);
    Event newevent = (Event)(ctx->events & ~event);
    // 判断取消当前事件事件是否为空，如果为空会将事件从epoll中删除
    int op = newevent ?EPOLL_CTL_MOD:EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.data.fd = fd;
    ep_event.events = EPOLLET | newevent;

    ctx->events = newevent;

    int rt2 = epoll_ctl(epfd, op, fd, &ep_event);
    if (rt2)
    {
        return false;
        //错误处理
    }

    return true;

}

bool IOmanager::delAndRunEvent(int fd, IOmanager::Event event){

    ReadMutexLock<RWMutex> mutex(&rwmutex);
    if(fd >= (int)event_ctx_list.size()){
        return false;
    }
    event_ctx* ctx = event_ctx_list[fd];
    mutex.unlock();
    if((event & ctx->events) == 0) return;
    MutexLock<Mutex> mutex2(&ctx->m_mutex);
    Event newevent = (Event)(ctx->events & ~event);

    int op = newevent ?EPOLL_CTL_MOD:EPOLL_CTL_DEL;
    epoll_event ep_event;
    ep_event.data.fd = fd;
    ep_event.events = EPOLLET | newevent;
    ctx->events = newevent;

    int rt2 = epoll_ctl(epfd, op, fd, &ep_event);
    if (rt2)
    {
        return false;
        //错误处理
    }

    ctx->triggerEvent(event);
    return true;
}

void IOmanager::idle(){

    //创建空间存储返回的epoll事件，最多一次接受256个事件
    const uint64_t MAX_EVNETS = 256;
    epoll_event* event_get = new epoll_event[MAX_EVNETS];
    std::shared_ptr<epoll_event> shared_events(event_get, [](epoll_event* ptr){
        delete[] ptr;
    });
    while(true){
        int rt = epoll_wait(epfd, event_get, MAX_EVNETS, -1);
        for (int i = 0; i < rt; i++)
        {
            epoll_event& cur_event = event_get[i];
            int fd = cur_event.data.fd;
            event_ctx* cur_event_ctx = (event_ctx*)cur_event.data.ptr;
            MutexLock<Mutex> mutex2(&cur_event_ctx->m_mutex);
            // 如果出现了EPOLLERR | EPOLLHUP事件，默认将所有读写事件全部触发
            if(cur_event.events & (EPOLLERR | EPOLLHUP)) {
                cur_event.events |= (EPOLLIN | EPOLLOUT) & cur_event_ctx->events;
            }
            // 触发读事件，触发后将读事件取消
            if (cur_event.events & EPOLLIN)
            {
                cur_event_ctx->triggerEvent(READ);
                cur_event_ctx->events = (Event)(cur_event_ctx->events & ~READ);
            }
            // 触发写事件，触发后将写事件取消
            if (cur_event.events & EPOLLOUT)
            {
                cur_event_ctx->triggerEvent(WRITE);
                cur_event_ctx->events = (Event)(cur_event_ctx->events & ~WRITE);
            }
            // 取消已触发事件，如果没有事件则在epoll中删除该事件
            int op = cur_event_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            cur_event.events = EPOLLET | cur_event_ctx->events;
            int rt2 = epoll_ctl(epfd, op, fd, &cur_event);
            if (rt2)
            {
                //错误处理
            }
            
        }
        // 每epoll_wait一次，就会回到主协程开始调度一次。
        Fiber::YieldToReady();
        

    }

}







}