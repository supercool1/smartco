#ifndef SMARTOR_NOCOPY_H_
#define SMARTOR_NOCOPY_H_

#include <pthread.h>

namespace smartco{

class Noallowcopy{
public:
    Noallowcopy() = default;
    Noallowcopy(Noallowcopy &T) = delete;
    Noallowcopy& operator = (const Noallowcopy &T)  = delete;
};

}





#endif