#ifndef SMARTOR_FIBER_H_
#define SMARTOR_FIBER_H_

#include <ucontext.h>
#include <memory>
#include <functional>
#include <atomic>
#include "util.h"
#define STACK_SIZE int(1<20)
namespace smartco{




class Fiber : public std::enable_shared_from_this<Fiber>
{

public:
enum status{
    READY, // 就绪可以执行状态
    HOLD, // 阻塞状态
    EXCEPT // 异常状态
};
typedef std::shared_ptr<Fiber> ptr;
    Fiber();
    Fiber(std::function<void()> cb, size_t stacksize = 4096);
    void swapin();
    void swapout();
    void setid();
    void setstatus(status);
    status getstatus();
    static void run();
    ucontext_t * get_ctx();
    Fiber::ptr get_fiber_ptr();
    ~Fiber();
    static void set_cur_fiber(Fiber::ptr f);
    //获取当前协程
    static Fiber::ptr get_cur_fiber();
    static void Yield();
    static void YieldToReady();
    static void CreateMainFiber();


private:
    ucontext_t m_context_t;
    std::string m_fiber_id;
    void* m_stack = nullptr;
    status m_status;
    std::function<void()>  m_cb;
    uint32_t m_stack_size;

};







}



#endif