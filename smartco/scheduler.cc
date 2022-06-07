#include "scheduler.h"
#include <assert.h>



namespace smartco{

void Scheduler::init(int thread_nums, bool use_main_thread, const std::string& name){
    this->thread_nums = thread_nums;
    this->use_main_thread = use_main_thread;
    this->m_name = name;
}

void Scheduler::scheduler(Fiber::ptr m_fiber, int thread_id, int stacksize){
    // 被调度的时候说明就绪了
    m_fiber->setstatus(Fiber::status::READY);
    task m_task;
    m_task.m_fiber = m_fiber;
    m_task.thread_id = thread_id;
    m_task.stacksize = stacksize;
    if(thread_nums == 1){
        // 单线程不加锁
        schedulerunlock(m_task);
    }else{
        Mutex::N_Mutex _mutex(&m_mutex);
        schedulerunlock(m_task);
    }
}

void Scheduler::scheduler(std::function<void()> cb, int thread_id, int stacksize){
    task m_task;
    m_task.cb = cb;
    m_task.thread_id = thread_id;
    m_task.stacksize = stacksize;
    if(thread_nums == 1){
        // 单线程不加锁
        schedulerunlock(m_task);
    }else{
        Mutex::N_Mutex _mutex(&m_mutex);
        schedulerunlock(m_task);
    }
}

void Scheduler::schedulerunlock(task & m_task){
    task_list.push_back(m_task);

}

void Scheduler::start(){
    // 如果主线程也用来调度的话，申请的线程数量减1
    int thread_remain = thread_nums;
    if(use_main_thread){
        thread_remain--;
    }
    thread_list.resize(thread_remain);
    for(int i = 0; i<thread_remain; i++){
        thread_list[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
    }
    if(use_main_thread){
        run();
    }
}

void Scheduler::run(){
    //初始化idel协程，当任务队列为空的时候执行该协程
    m_idel_fiber.reset(new Fiber(std::bind(&Scheduler::idle, this)));
    task m_task;
    while(true){
        // 加括号是为了控制锁的生存周期
        {
            Mutex::N_Mutex _mutex(&m_mutex);
            auto it = task_list.begin();
            while(it != task_list.end()){
                if(it->thread_id != -1 && smartco::GetThreadId() != it->thread_id){
                    // 提醒一下其他线程处理这个事件
                    ticket();
                    it++;
                    continue;
                }
                m_task = *it;
                task_list.erase(it++);
                break;
            }
        }
        // 判断协程实例是否存在，将协程实例放在这里生成，有助于性能分散到多线程上
        if(m_task.cb){
            m_task.m_fiber.reset(new Fiber(m_task.cb));
        }

        //开始调用协程,如果协程就绪则执行，其余情况丢弃，规定只有就绪的协程才能进入执行队列
        if(m_task.m_fiber.get()!= nullptr && m_task.m_fiber->getstatus() == Fiber::status::READY){
            m_task.m_fiber->swapin();
            // 如果协程执行后仍为就绪，那么将它放入执行队列，不太理解这一步的实际用途，如果一直就绪为什么要切回主线程。
            if( m_task.m_fiber->getstatus() == Fiber::status::READY){
                scheduler(m_task.m_fiber);
            }
        }else{
            //如果协程为空则代表任务队列为空，则进入idel协程
            m_idel_fiber->swapin();
        }
        // 清除当前任务状态
        m_task.clear();
 
    }
    
}

void Scheduler::idle(){
    while(true){
        sleep(1);
        printf("idel\n");
        smartco::Fiber::Yield();
    }
    
}

void Scheduler::ticket(){

}

}