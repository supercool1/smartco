#include "fiber.h"


namespace smartco{

static thread_local Fiber::ptr cur_fiber = nullptr;           // 当前协程
static thread_local Fiber::ptr cur_thread_main_Fiber = nullptr; // 主协程智能指针

Fiber::Fiber(){
    if(getcontext(&m_context_t)){
        throw std::logic_error("getcontext error");
    }
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize):m_cb(cb),m_stack_size(stacksize){

    if(!cur_thread_main_Fiber){
        Fiber::ptr main_fiber(new Fiber);
        cur_thread_main_Fiber = main_fiber;
    }

    if(getcontext(&m_context_t)){
        throw std::logic_error("getcontext error");
    }
    m_stack = malloc(stacksize);
    m_context_t.uc_link = nullptr;
    m_context_t.uc_stack.ss_sp = m_stack;
    m_context_t.uc_stack.ss_size = stacksize;

    makecontext(&m_context_t, &Fiber::run, 0);

}
void Fiber::swapin(){
    set_cur_fiber(get_fiber_ptr());
    swapcontext(cur_thread_main_Fiber->get_ctx(), &m_context_t);
}

void Fiber::swapout(){
    set_cur_fiber(cur_thread_main_Fiber->get_fiber_ptr());
    swapcontext(&m_context_t, cur_thread_main_Fiber->get_ctx());
}

void Fiber::run(){
    Fiber::ptr cur = get_cur_fiber();
    cur->m_cb();
    cur->swapout();
}

Fiber::~Fiber(){
    free(m_stack);
}

ucontext_t* Fiber::get_ctx(){
    return &m_context_t;
}

void Fiber::set_cur_fiber(Fiber::ptr f){
    cur_fiber = f;
}

Fiber::ptr Fiber::get_cur_fiber(){
    return cur_fiber;
}

Fiber::ptr Fiber::get_fiber_ptr(){
    return shared_from_this();
}

void Fiber::Yield(){
    Fiber::ptr cur = get_cur_fiber();
    cur->swapout();
} 

Fiber::status Fiber::getstatus(){
    return m_status;
}


} 