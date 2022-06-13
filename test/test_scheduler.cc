#include "scheduler.h"

#include "thread.h"
#include "mutex.h"
#include "fiber.h"
#include "util.h"
#include <unistd.h>
#include <iostream>
#include <vector>
void fun1(){
    while(true){
        printf("fiber1\n");
        sleep(2);
        smartco::Fiber::YieldToReady();
    }
}
void fun2(){
    while(true){
        printf("fiber2\n");
        sleep(2);
        smartco::Fiber::YieldToReady();
    }
}

void fun4(){
    while(true){
        printf("fiber4\n");
        sleep(2);
        smartco::Fiber::YieldToReady();
    }
}

void fun3(){
    smartco::Scheduler::get_cur_scheduler()->scheduler(fun4);
    while(true){
        printf("fiber3\n");
        sleep(2);
        smartco::Fiber::YieldToReady();
    }
}

class A{
public:
    A(){
        ss.push_back(1);
        ss.push_back(2);
        ss.push_back(3);
        ss.push_back(4);
    };
    void run(){
        p.reset(new smartco::Fiber(&fun3, 10240));
        p->swapin();
        for(int j = 0; j<4; j++){
            printf("ss: %d  \n", ss[j]);
        }
        p->swapin();
        for(int j = 0; j<4; j++){
            printf("ss: %d  \n", ss[j]);
        }

    }
    smartco::Fiber::ptr p;
    std::vector<int> ss;
};

int main(){
    smartco::Scheduler* m_scheduler = new smartco::Scheduler();
    m_scheduler->scheduler(fun1);
    m_scheduler->scheduler(fun2);
    m_scheduler->scheduler(fun3);
   
    m_scheduler->start();

    // A a = A();
    
    // a.run();
}

