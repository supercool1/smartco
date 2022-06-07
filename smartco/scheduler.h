#ifndef SMARTOR_SCHEDULER_H_
#define SMARTOR_SCHEDULER_H_
#include "fiber.h"
#include "singleton.h"
#include "thread.h"
#include "mutex.h"
#include "util.h"
#include <vector>
#include <list>
namespace smartco{

class Scheduler:public Singleton<Scheduler>
{
private:
    
public:
    void init(int thread_nums = 1, bool use_main_thread = true, const std::string& name = "undefine");
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
        

    };
    void scheduler(Fiber::ptr m_fiber, int thread_id = -1, int stacksize = 0);
    void scheduler(std::function<void()> cb, int thread_id = -1, int stacksize = 0);
    void schedulerunlock(task & m_task);
    void run();
    void start();
    void ticket();
    // 是否正在停止
    bool isstopping(){
        //待实现

        return false;
    }
    virtual void idle();


private:
    std::string m_name;
    //list是链表实现的，删除其中某个成员成本比vector小
    std::list<task> task_list;    //任务列表
    // std::vector<task> task_list;
    int thread_nums = 0;
    bool use_main_thread = true;
    std::vector<Thread::ptr> thread_list;
    Mutex m_mutex;
    Fiber::ptr m_idel_fiber;
};



}


#endif