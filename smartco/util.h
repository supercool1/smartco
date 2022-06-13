#ifndef SMARTOR_UTIL_H_
#define SMARTOR_UTIL_H_

#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string>
#include<assert.h>

namespace smartco{
    pid_t GetThreadId();

    void SetFiberId(std::string& t_str);

    void SetNonblocking(int fd);

    void DelNonblocking(int fd);

}
#endif