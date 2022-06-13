#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <memory>
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
    int filedes[2];
    int rt = 0;
    rt = pipe(filedes);
    if(rt == 0){
        printf("new pipe success %d %d\n", filedes[0], filedes[1]);
    }
    setnonblocking(filedes[0]);
    setnonblocking(filedes[1]);
    
    // 写入数据
    char str[11] = "hello word";

    int time = 6000;
    while(time--){
        rt = write(filedes[1], str, 11);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        printf("write size : %d\n", rt);
    }

    close(filedes[0]);
    close(filedes[1]);
}

// 写事件的触发
void writeeven(){
    int efd = epoll_create(1000);
    int filedes[2];
    int rt = 0;
    rt = pipe(filedes);
    if(rt == 0){
        printf("new pipe success %d %d\n", filedes[0], filedes[1]);
    }
    setnonblocking(filedes[0]);
    setnonblocking(filedes[1]);
    epoll_event event;
    event.data.fd = filedes[1];
    event.events = EPOLLOUT|EPOLLET;
    epoll_ctl(efd, EPOLL_CTL_ADD, filedes[1], &event);
    epoll_event event_get[10];

    char str[11] = "hello word";
    while (1)
    {
        rt = epoll_wait(efd, event_get, 10, -1);
        printf("rec %d events\n", rt);
        rt = write(filedes[1], str, 11);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        char str2[11];
        rt = read(filedes[0], str2, 11);
        printf("write size : %d\n", rt);
    }
    

    close(filedes[0]);
    close(filedes[1]);
    close(efd);
}

// 写事件的触发
void readeven(){
    int efd = epoll_create(1000);
    int filedes[2];
    int rt = 0;
    rt = pipe(filedes);
    if(rt == 0){
        printf("new pipe success %d %d\n", filedes[0], filedes[1]);
    }
    // setnonblocking(filedes[0]);
    // setnonblocking(filedes[1]);
    epoll_event event;
    event.data.fd = filedes[0];
    event.events = EPOLLIN|EPOLLET;
    epoll_ctl(efd, EPOLL_CTL_ADD, filedes[0], &event);
    epoll_event event_get[10];

    while (1)
    {
        // rt = write(filedes[1], str, 11);
        rt = epoll_wait(efd, event_get, 10, 1000);
        printf("rec %d events\n", rt);
        if(rt <= 0){
            printf("write error %d %d \n", errno, EAGAIN);
        }
        // char str2[11];
        // rt = read(filedes[0], str2, 11);
        // printf("write size : %d\n", rt);
    }
    

    close(filedes[0]);
    close(filedes[1]);
    close(efd);
}
class test_iomanager
{
private:
    /* data */
public:
    test_iomanager(/* args */);
    ~test_iomanager();
};

test_iomanager::test_iomanager(/* args */)
{
}

test_iomanager::~test_iomanager()
{
}


int main(){
    std::shared_ptr<test_iomanager> a(nullptr);
    if(a){
        std::cout<<"asdads"<<std::endl;
    }else{
        std::cout<<"ddddddd"<<std::endl;
    }
}