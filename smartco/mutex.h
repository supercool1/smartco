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
private:
    T* m_mutex;
    bool is_lock;
};

template<class T>
class ReadMutexLock{
public:
    ReadMutexLock(T* mutex):m_mutex(mutex){
        m_mutex->rdlock();
    }

    ~ReadMutexLock(){
        m_mutex->unlock();
    }
    void lock(){
        if(!is_lock){
            m_mutex->lock();
            is_lock = true;
        }
    }

    void unlock(){
        if(is_lock){
            m_mutex->unlock();
            is_lock = false;
        }
    }
private:
    T* m_mutex;
    bool is_lock;
};

template<class T>
class WriteMutexLock{
public:
    WriteMutexLock(T* mutex):m_mutex(mutex){
        m_mutex->wrlock();
        is_lock = true;
    }

    ~WriteMutexLock(){
        m_mutex->unlock();
    }
    
    void lock(){
        if(!is_lock){
            m_mutex->lock();
            is_lock = true;
        }
    }

    void unlock(){
        if(is_lock){
            m_mutex->unlock();
            is_lock = false;
        }
    }

private:
    T* m_mutex;
    bool is_lock;
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


class RWMutex : Noallowcopy{
public:

    /// 局部读锁
    typedef ReadMutexLock<RWMutex> ReadLock;

    /// 局部写锁
    typedef WriteMutexLock<RWMutex> WriteLock;

    /**
     * @brief 构造函数
     */
    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }
    
    /**
     * @brief 析构函数
     */
    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    /**
     * @brief 上读锁
     */
    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    /**
     * @brief 上写锁
     */
    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    /// 读写锁
    pthread_rwlock_t m_lock;
};

}



#endif