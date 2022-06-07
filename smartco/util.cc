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

}

