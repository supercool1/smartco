#include "thread.h"


namespace smartco{
    
    Thread::Thread(std::function<void()> cb, const std::string& name): m_cb(cb), m_name(name){
        if(name.empty()){
            m_name = "UNKNOW";
        }
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if(!rt){
            //问题处理
        }  
    };

    void* Thread::run(void* arg){
        Thread* cur = (Thread*)arg;
        cur->m_cb();

        return nullptr;
    }

    int Thread::join(){
        return pthread_join(m_thread, nullptr);
    }
    bool Thread::joinable(){
        return false;
    }
    int Thread::detach(){
        return pthread_detach(m_thread);
    }


}