//
// Created by util on 2021/10/3.
//

#ifndef HELLOSERV_THREADPOOL_H
#define HELLOSERV_THREADPOOL_H

#include <pthread.h>
#include <functional>

class Task
{
    void (*func)(void* arg);
    void* arg;
};

class threadPool {
    class Task* taskQueue;
    int queueCapacity;
    int queueSize;
    int queueFront;
    int queueRear;

    pthread_t managerThreadPid;
    pthread_t* workerThreadPid;
    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum;

    pthread_mutex_t mutexPool;
    pthread_mutex_t mutexBusyN;
    pthread_cond_t full;
    pthread_cond_t empty;

    int isShutdown;
};



#endif //HELLOSERV_THREADPOOL_H
