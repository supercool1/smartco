/**
 * @file iomanager.h
 * @brief 封装了epoll的协程调度器
 * @author 
 * @email 1026486701@qq.com
 * @date 2022-06-09
 * @copyright 
 */

#ifndef SMARTOR_IOMANAGER_H_
#define SMARTOR_IOMANAGER_H_

#include "scheduler.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

namespace smartco{

class IOmanager : public Scheduler
{

public:
    enum Event{
        /// 无事件
        NONE    = 0x0,
        /// 读事件(EPOLLIN)
        READ    = 0x1,
        /// 写事件(EPOLLOUT)
        WRITE   = 0x4,
    };

    struct event_ctx
    {
        
         /// 读事件上下文
        task read;
        /// 写事件上下文
        task write;
        /// 事件关联的句柄
        int fd = 0;
        /// 当前的事件
        Event events = NONE;
        /// 事件的Mutex
        Mutex m_mutex;

        task* getCtx(Event events){
            if(events == READ){
                return &read;
            }else if (events == WRITE)
            {
                return &write;
            }
            return nullptr;
        }

        void triggerEvent(Event event){

            task* m_task = getCtx(event);
            events = (Event)(events&~event);
            if(m_task->cb){
                get_cur_scheduler()->scheduler(m_task->cb);
            }else if(m_task->m_fiber){
                get_cur_scheduler()->scheduler(m_task->m_fiber);
            }
        }

    };
    
    IOmanager(int thread_nums = 1, bool use_main_thread = true, const std::string& name = "undefine");
    ~IOmanager();

    bool addEvent(int fd, IOmanager::Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, IOmanager::Event event);
    bool delAndRunEvent(int fd, IOmanager::Event event);

    void idle();

    static IOmanager* get_cur_iomanager();
private:
    void readAll();
    void event_ctx_list_resize(int size);
    int epfd;
    std::vector<event_ctx*> event_ctx_list;
    RWMutex rwmutex;
    int filedes[2];
};








}


#endif