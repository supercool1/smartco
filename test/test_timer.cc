#include "timer.h"
#include "util.h"
#include <unistd.h>
#include <iostream>
#include <vector>

void fun(){
    printf("aaa\n");
}

int main(){

    smartco::Timermanager* t = new smartco::Timermanager();
    t->addtimer(4000, fun);
    t->addtimer(2000, fun);
    t->addtimer(2000, fun);
    while (1)
    {
        uint64_t tt = 1000;
        tt = t->get_next_time();
        t->del_time_out();
        printf("left time = %lu\n", tt);
        sleep(1);
    }
    

}