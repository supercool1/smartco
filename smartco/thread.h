#ifndef SMARTOR_THREAD_H_
#define SMARTOR_THREAD_H_

#include <pthread.h>
#include <functional>
#include "nocopy.h"
namespace smartco{
void dunn(){
    printf("ADASD\n");
}

class Thread : public Noallowcopy{
public:
    Thread(std::function<void()> cb, const std::string& name);

    static void* run(void* arg);

    int join();
    bool joinable();
    int detach();
    std::string& get_threadid() {
        return m_name;
    };

private:
    pthread_t m_thread = 0;;
    std::function<void()> m_cb;
    std::string m_name;

};






}





#endif