// 因为把fiber数据成员和iomanager的方法包含在了调度器内，感觉写的导致这几个模块的耦合性高了
// 但是本意上是实现fiber的定时唤醒功能（不知道这个功能的添加产生的优势和耦合产生的劣势那个更重要一点）

#ifndef SMARTOR_Timer_H_
#define SMARTOR_Timer_H_

#include "scheduler.h"
#include <sys/time.h>
#include <set>
#include <vector>
namespace smartco{

// epoll最长等待时间
#define MAX_WAIT_TIME 3000


class Timermanager;

class Timer : public Noallowcopy, public std::enable_shared_from_this<Timer>
{
friend class Timermanager;
public:
    typedef std::shared_ptr<Timer> ptr;
    // 取消定时器
    bool cancel();
    // 重新设置执行时间
    bool reset(uint64_t ms, bool start_now = false);
    // 周期定时器用于刷新周期,以相同的ms再次加入定时器列表
    bool refresh();
    // 判断该定时器是否有效
    bool isvalid();
private:
    // 设置该定时器失效 
    void setinvalid();
    // 设置该定时器有效
    void setvalid();
    // 当传入cb = nullptr的时候，会自动构建定时唤醒协程的定时器，当然，这不能是周期循环的。
    Timer(uint64_t ms, Timermanager *timermanager, std::function<void()> cb = nullptr, bool recurring = false);
    // 获取触发时间
    uint64_t get_trigger_time();
    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };

    // 管理当前对象的管理器
    Timermanager *m_timermanager;
    // 是否是周期循环定时器
    bool cycle;
    // 延后执行时间
    uint64_t m_ms;
    // 精确执行时间
    uint64_t trigger_ms;
    // 函数回调和协程智能指针，两者只能包含一个
    std::function<void()> m_cb;
    Fiber::ptr m_fiber;
    // 当前定时器是否处于计时状态
    bool use;
};

class Timermanager : public Noallowcopy
{
friend class Timer;
public:
    Timermanager(){};
    
    // 增加定时器
    Timer::ptr addtimer(uint64_t ms, std::function<void()> cb = nullptr, bool recurring = false);

    // 获取最近将要超时的计时器时间间隔，如果有计时器已经超时，则返回0
    uint64_t get_next_time();
    // 处理超时计时器，将超时定时器传入调度器调度队列中，当前接口耦合度过高，以后可能修改
    void del_time_out();
private:
    // 计时队列
    std::set<Timer::ptr, Timer::Comparator> timer_list;
    RWMutex rwmutex;
};





}

#endif  