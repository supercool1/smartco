#include "timer.h"
#include "util.h"


namespace smartco{

bool Timer::Comparator::operator()(const Timer::ptr& lhs
                        ,const Timer::ptr& rhs) const {
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->trigger_ms < rhs->trigger_ms) {
        return true;
    }
    if(rhs->trigger_ms < lhs->trigger_ms) {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, Timermanager *timermanager, std::function<void()> cb, bool cycle):
            m_timermanager(timermanager),
            cycle(cycle),
            m_ms(ms),
            m_cb(cb),
            use(true)
{
    // 设定精准触发时间
    trigger_ms = GetCurTime() + ms;
    if(cb == nullptr){
        // 如果传入的是协程，想在某时刻后进行调度，则会主动让出该协程执行权
        m_fiber = Fiber::get_cur_fiber();
        Fiber::Yield();
    }
}

uint64_t Timer::get_trigger_time(){
    return trigger_ms;
}

bool Timer::cancel(){
    WriteMutexLock<RWMutex> mutex(&m_timermanager->rwmutex);
    if(use) {
        use = false;
        m_cb = nullptr;
        m_fiber.reset();
        auto it = m_timermanager->timer_list.find(shared_from_this());
        m_timermanager->timer_list.erase(it);
        return true;
    }
    return false;

}

bool Timer::reset(uint64_t ms, bool start_now){
    // 如果没有修改，直接返回true
    if(ms == m_ms && !start_now) {
        return true;
    }

    WriteMutexLock<RWMutex> mutex(&m_timermanager->rwmutex);
    if(use) {
        auto it = m_timermanager->timer_list.find(shared_from_this());
        m_timermanager->timer_list.erase(it);
        uint64_t cur_time = GetCurTime();
        if(start_now) {
            trigger_ms = cur_time + ms;
        } else {
            trigger_ms = trigger_ms - m_ms + ms;
        }
        m_ms = ms;
        m_timermanager->timer_list.insert(shared_from_this());
        return true;
    }
    return false;
}

bool Timer::refresh(){
    // 将协程重新设置为有效
    setvalid();
    return reset(m_ms, true);
}

bool Timer::isvalid(){
    return use;
}

void Timer::setinvalid(){
    use = false;
}

void Timer::setvalid(){
    use = true;
}

Timer::ptr Timermanager::addtimer(uint64_t ms, std::function<void()> cb, bool cycle){
    Timer::ptr m_timer(new Timer(ms, this, cb, cycle));

    // 不能执行周期的协程定时器，因为如果定时时间短，一个协程会被多个线程访问
    if (cb == nullptr && cycle == true)
    {
        m_timer.reset();
        return m_timer;
    }
    // 修改timer_list，上写锁
    WriteMutexLock<RWMutex> mutex(&rwmutex);
    timer_list.insert(m_timer);
    return m_timer;
}

uint64_t Timermanager::get_next_time(){
    // 获取当前时间
    uint64_t cur_time = GetCurTime();
    uint64_t wait_t = 0;
    ReadMutexLock<RWMutex> mutex(&rwmutex);
    auto it = timer_list.begin();
    // 如果定时器内没有任务，则将epoll设置为最长等待时间
    if(it == timer_list.end()){
        return MAX_WAIT_TIME;
    }else{
        wait_t = (*it)->get_trigger_time() - cur_time;
        wait_t = wait_t>0?wait_t:0;
    }
    // 最大等待时间为MAX_WAIT_TIME，
    return wait_t < MAX_WAIT_TIME ? wait_t : MAX_WAIT_TIME;
}

void Timermanager::del_time_out(){
    
    uint64_t cur_time = GetCurTime();
    WriteMutexLock<RWMutex> mutex(&rwmutex);
    auto it = timer_list.begin();
    
    while (it != timer_list.end())
    {   
        //获取超时时间
        uint64_t wait_t = (*it)->get_trigger_time() - cur_time;
        if (wait_t > 0)
        {
            break;
        }
        // 将事件放入调度器中
        if((*it)->m_cb){
            Scheduler::get_cur_scheduler()->scheduler((*it)->m_cb);
            // 如果是周期循环事件，则再将其放入计时队列
            if ((*it)->cycle)
            {
                (*it++)->refresh();
            }
            
        }else if((*it)->m_fiber){
            Scheduler::get_cur_scheduler()->scheduler((*it)->m_fiber);
            (*it)->setinvalid();
            timer_list.erase(it++);
        }

        
    }
}


}