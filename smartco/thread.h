#ifndef SMARTOR_THREAD_H_
#define SMARTOR_THREAD_H_

// #include <pthread.h>
#include <functional>
#include <memory>
#include "nocopy.h"

namespace smartco{

class Thread : public Noallowcopy{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);

    static void* run(void* arg);

    int join();
    int detach();
    std::string& get_thread_name() {
        return m_name;
    };
    pthread_t get_thread_id(){
        return m_thread;
    }
private:
    pthread_t m_thread = 0;;
    std::function<void()> m_cb;
    std::string m_name;
    
};






}





#endif