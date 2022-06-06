#include "thread.h"
#include "mutex.h"
#include <unistd.h>
#include <iostream>

smartco::Mutex m_mutex;
smartco::Semaphore m_semaphore;
// class A{
//     public:
//     A(){};
//     ~A(){};


// };
void fun1(){

    printf("thread0\n");
    sleep(2);
    smartco::Mutex::N_Mutex _mutex(&m_mutex);
    printf("thread3\n");
    sleep(3);
    m_semaphore.notify();
}

void fun2(){
    sleep(1);
    printf("thread1\n");
    smartco::Mutex::N_Mutex _mutex(&m_mutex);
    printf("thread2\n");
    _mutex.unlock();
    m_semaphore.wait();
    printf("thread4\n");
}


int main(){
    // std::function<void()> cb = fun1;
    smartco::Thread *m_t = new smartco::Thread(fun1, "thread1");
    smartco::Thread *m_2 = new smartco::Thread(fun2, "thread2");
    
    m_t->join();
    m_2->join();

    return 0;

}