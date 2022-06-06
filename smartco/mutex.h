#ifndef SMARTOR_MUTEX_H_
#define SMARTOR_MUTEX_H_


#include <thread>
#include <memory>
#include <semaphore.h>
#include "nocopy.h"

namespace smartco{

template<class T>
class MutexLock
{
private:
    T* m_mutex;
public:
    MutexLock(T* mutex):m_mutex(mutex) {
        lock();
    }
    ~MutexLock(){
        unlock();
    }
    void lock(){
        m_mutex->lock();
    }

    void unlock(){
        m_mutex->unlock();
    }

    void trylock(){
         m_mutex->trylock();
    }
};


class Semaphore : public Noallowcopy{
public:
    /**
     * @brief 构造函数
     * @param[in] count 信号量值的大小
     */
    Semaphore(uint32_t count = 0);

    /**
     * @brief 析构函数
     */
    ~Semaphore();
    /**
     * @brief 获取信号量
     */
    void wait();
    

    /**
     * @brief 释放信号量
     */
    void notify();

private:
    sem_t m_semaphore;
};

class Mutex : public Noallowcopy{
public:
    typedef MutexLock<Mutex> N_Mutex;
    Mutex();
    ~Mutex();
    void lock();
    void trylock();
    void unlock();



private:
    pthread_mutex_t mutex;
    bool is_lock;
};


}



#endif