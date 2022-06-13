#include "util.h"
#include<iostream>

namespace smartco{
    pid_t GetThreadId(){
        return syscall(SYS_gettid);
    }

    void SetFiberId(std::string& t_str){
        // time_t t = time(0);
        // char ch[64];
        // strftime(ch, sizeof(ch), "%Y%m%d%H%M%S", localtime(&t)); //年-月-日 时-分-秒
        // t_str = ch;
        return;
    }

    void SetNonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
    }

    void DelNonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option & ~O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
    }

}

