#include "thread.h"
#include "mutex.h"
#include "fiber.h"
#include <unistd.h>
#include <iostream>



void fun2(){
    printf("fiber1\n");
    smartco::Fiber::Yield();
    printf("fiber3\n");
    smartco::Fiber::Yield();
}

int main(){

    smartco::Fiber::ptr m_fiber(new smartco::Fiber(&fun2, 10240));
    printf("fiber0\n");
    m_fiber->swapin();
    printf("fiber2\n");
    m_fiber->swapin();
    printf("fiber4\n");

}

// class D: public std::enable_shared_from_this<D>
// {
// public:
//     D()
//     {
//         std::cout<<"D::D()"<<std::endl;
//     }
    
//     void func()
//     {
//         std::cout<<"D::func()"<<std::endl;
//         std::shared_ptr<D> p = shared_from_this();
//     }    
// };
 
// int main()
// {
//     // std::shared_ptr<D> p(new D);
//     std::shared_ptr<smartco::Fiber> p(new smartco::Fiber);
//     p->get_fiber_ptr();
//     return 0;    
// }  