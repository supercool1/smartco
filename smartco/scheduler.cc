#include "scheduler.h"
#include <assert.h>



namespace smartco{
Scheduler::Scheduler(int thread_nums, bool use_main_thread, const std::string& name):
                    thread_nums(thread_nums),
                    use_main_thread(use_main_thread),
                    m_name(name){
    //初始化idel协程，当任务队列为空的时候执行该协程
    m_idel_fiber.reset(new Fiber(&Fiber::idel));


}
Scheduler::~Scheduler()
{

}

void Scheduler::scheduler(Fiber::ptr m_fiber){
    // 只有就绪协程才能放入被执行队列
    assert(m_fiber->getstatus() == Fiber::status::READY);
    Mutex::N_Mutex _mutex(&m_mutex);
    task m_task;
    m_task.m_fiber = m_fiber;
    task_list.push_back(m_task);
}
void Scheduler::scheduler(std::function<void()> cb){

}

void Scheduler::start(){
    thread_list.resize(thread_nums);
    for(int i = 0; i<thread_nums; i++){
        thread_list[i].reset(new Thread(std::bind(Scheduler::run, this), m_name + "_" + std::to_string(i)));
    }
}

void Scheduler::run(){
    //初始化idel协程，当任务队列为空的时候执行该协程
    m_idel_fiber.reset(new Fiber(std::bind(&Fiber::idle, this)));
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
                    continue;
                }
                m_task = *it;
                task_list.erase(it);
                break;
            }
        }
        // 判断协程实例是否存在，将协程实例放在这里生成可以，有助于性能分散到多线程上
        if(m_task.cb){
            m_task.m_fiber.reset(new Fiber(m_task.cb, m_task.stacksize));
        }

        //开始调用协程,如果协程就绪则执行，其余情况丢弃，规定只有就绪的协程才能进入执行队列
        if(m_task.m_fiber && m_task.m_fiber->getstatus() == Fiber::status::READY){
            m_task.m_fiber->swapin();
            // 如果协程执行后仍为就绪，那么将它放入执行队列，不太理解这一步的实际用途，如果一直就绪为什么要切回主线程。
            if( m_task.m_fiber->getstatus() == Fiber::status::READY){
                scheduler(m_task.m_fiber);
            }
        }else{
            //如果协程为空则代表任务队列为空，则进入idel协程
            m_idel_fiber->swapin();
        }

        m_task.clear();



        
        
    }
    
}

void Scheduler::idle(){
    while(true){
        sleep(1);
        smartco::Fiber::Yield();
    }
    
}



}