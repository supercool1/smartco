#ifndef SMARTOR_UTIL_H_
#define SMARTOR_UTIL_H_

#include <unistd.h>
#include <sys/syscall.h>

namespace smartco{
    pid_t GetThreadId();

}
#endif