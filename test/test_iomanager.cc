#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <memory>
#include "iomanager.h"
// #include "scheduler.h"

// 设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
// 测试写阻塞(非阻塞)，当缓冲区满的时候，写就会阻塞(返回EAGAIN)
void writeblock(){
    int file[2];
    int rt = 0;
    rt = pipe(file);
    if(rt == 0){
        printf("new pipe success %d %d\n", file[0], file[1]);
    }
    setnonblocking(file[0]);
    setnonblocking(file[1]);
    
    // 写入数据
    char str[11] = "hello word";

    int time = 6000;
    while(time--){
        rt = write(file[1], str, 11);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        printf("write size : %d\n", rt);
    }

    close(file[0]);
    close(file[1]);
}

// 写事件的触发
void writeeven(){
    int efd = epoll_create(1000);
    int file[2];
    int rt = 0;
    rt = pipe(file);
    if(rt == 0){
        printf("new pipe success %d %d\n", file[0], file[1]);
    }
    setnonblocking(file[0]);
    setnonblocking(file[1]);
    epoll_event event;
    event.data.fd = file[1];
    event.events = EPOLLOUT|EPOLLET;
    epoll_ctl(efd, EPOLL_CTL_ADD, file[1], &event);
    epoll_event event_get[10];

    char str[11] = "hello word";
    while (1)
    {
        rt = epoll_wait(efd, event_get, 10, -1);
        printf("rec %d events\n", rt);
        rt = write(file[1], str, 11);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        char str2[11];
        rt = read(file[0], str2, 11);
        printf("write size : %d\n", rt);
    }
    

    close(file[0]);
    close(file[1]);
    close(efd);
}

// 写事件的触发
void readeven(){
    int efd = epoll_create(1000);
    int file[2];
    int rt = 0;
    rt = pipe(file);
    if(rt == 0){
        printf("new pipe success %d %d\n", file[0], file[1]);
    }
    // setnonblocking(file[0]);
    // setnonblocking(file[1]);
    epoll_event event;
    event.data.fd = file[0];
    event.events = EPOLLIN|EPOLLET;
    epoll_ctl(efd, EPOLL_CTL_ADD, file[0], &event);
    epoll_event event_get[10];

    while (1)
    {
        // rt = write(file[1], str, 11);
        rt = epoll_wait(efd, event_get, 10, 1000);
        printf("rec %d events\n", rt);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        // char str2[11];
        // rt = read(file[0], str2, 11);
        // printf("write size : %d\n", rt);
    }
    

    close(file[0]);
    close(file[1]);
    close(efd);
}


// 测试代码
int file[2];
smartco::IOmanager* iomanager = nullptr;

void fun2(){
    while (1)
    {   
        sleep(1);
        smartco::IOmanager::get_cur_iomanager()->addEvent(file[0], smartco::IOmanager::READ);
        printf("trigger read fun\n");
        uint8_t dummy[256];
        while(read(file[0], dummy, sizeof(dummy)) > 0);      
    }
}


void fun3(){
    printf("read fun3\n");
    char str[11] = "hello word";
    while (1)
    {
        sleep(1);
        smartco::IOmanager::get_cur_iomanager()->addEvent(file[1], smartco::IOmanager::WRITE);
        printf("trigger write fun\n");
        write(file[1], str, 11); 
    }
}
void test_fiber_wake_up(){
    pipe(file);
    setnonblocking(file[0]);
    setnonblocking(file[1]);
    iomanager = new smartco::IOmanager();
    iomanager->scheduler(fun2);
    iomanager->scheduler(fun3);
    iomanager->start();
}

void fun_cb2(){
    printf("trigger fun_cb2\n");
}

void fun_cb(){
    char str[11] = "hello word";

    printf("trigger funcb\n");
    smartco::IOmanager::get_cur_iomanager()->addEvent(file[0], smartco::IOmanager::READ, &fun_cb2);
    write(file[1], str, 11); 
    smartco::Fiber::Yield();
    
}


void test_fun_cb(){
    pipe(file);
    setnonblocking(file[0]);
    setnonblocking(file[1]);
    iomanager = new smartco::IOmanager();
    iomanager->scheduler(fun_cb);
    iomanager->start();
}



int main(){
    test_fun_cb();

}