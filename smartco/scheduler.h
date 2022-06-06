#ifndef SMARTOR_SCHEDULER_H_
#define SMARTOR_SCHEDULER_H_
#include "fiber.h"
#include "thread.h"
#include "mutex.h"
#include "util.h"
#include <vector>
#include <list>
namespace smartco{

class Scheduler
{
private:
    /* data */
public:
    Scheduler(int thread_nums = 1, bool use_main_thread = true, const std::string& name = "undefine");
    ~Scheduler();
    struct task{
        Fiber::ptr m_fiber;       
        std::function<void()> cb;
        size_t stacksize = 0;
        int thread_id = -1;

        task(){
            clear();
        }

        void clear(){
            m_fiber.reset();
            cb = nullptr;
            stacksize = 0;
            thread_id = -1;
        }
        task& operator = (task& m_task){
            m_fiber = m_task.m_fiber;
            cb = m_task.cb;
            stacksize = m_task.stacksize;
            thread_id = m_task.thread_id;
        }

    };
    void scheduler(Fiber::ptr m_fiber);
    void scheduler(std::function<void()> cb);
    void run();
    void start();
    void ticket();
    virtual void idle();


private:
    std::string m_name;
    //list是链表实现的，删除其中某个成员成本比vector小
    std::list<task> task_list;    //任务列表
    int thread_nums;
    bool use_main_thread;
    std::vector<Thread::ptr> thread_list;
    Mutex m_mutex;
    Fiber::ptr m_idel_fiber;
};



}


#endif