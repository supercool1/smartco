#include "util.h"


namespace smartco{
    pid_t GetThreadId(){
        return syscall(SYS_gettid);
    }

}

