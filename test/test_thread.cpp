#include "thread.h"

void fun1(){

    printf("thread\n");
}


int main(){
    // std::function<void()> cb = fun1;
    smartco::Thread *m_t = new smartco::Thread(fun1, "thread1");
    smartco::dunn();
    m_t->join();

    return 0;

}