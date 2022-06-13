#include "mutex.h"


namespace smartco{

Semaphore::Semaphore(uint32_t count){
    sem_init(&m_semaphore, 0, count);
}


Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}
/**
 * @brief 获取信号量
 */
void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        throw std::logic_error("sem_wait error");
    }
}


/**
 * @brief 释放信号量
 */
void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        throw std::logic_error("sem_post error");
    }
}



Mutex::Mutex(){
    pthread_mutex_init(&mutex, nullptr);
    is_lock = false;
}


Mutex::~Mutex(){
    unlock();
    pthread_mutex_destroy(&mutex);
}


void Mutex::lock(){
    if(!is_lock){
        pthread_mutex_lock(&mutex);
        is_lock = true;
    }
}


void Mutex::trylock(){
    if(!is_lock && !pthread_mutex_trylock(&mutex)){
        is_lock = true;
    }
}

void Mutex::unlock(){
     if(is_lock){
        pthread_mutex_unlock(&mutex);
        is_lock = false;
    }
}



}