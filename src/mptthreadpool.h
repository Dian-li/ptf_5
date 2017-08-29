/**
*
* @file      mptthreadpool.h
* @brief     define MPT worker thread pool
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/08/08,  Initial version
*/

#ifndef _MPTTHREADPOOL_H
#define _MPTTHREADPOOL_H

#include <pthread.h>
#include <event.h>

#include "ringbuff.h"
#include "mptevent.h"

struct TThreadData
{
    pthread_t  m_threadID;
    struct event_base *m_base;
    struct event m_notifyEvent;
    int m_upwardFd;
    int m_downwardFd;
    TRingbuffer<TMTPEvent> *m_queue;
};

class TMptThreadPool
{
public:
    TMptThreadPool();
    ~TMptThreadPool();
    int init(int nThread);

private:
    static void* workerFunc(void* arg);
    static void threadEventProcess(int fd, short which, void *arg);
    int setupThread(TThreadData* data);
    int createThread(void *(*func)(void *), TThreadData *arg);

private:
    int m_nCount;
    TThreadData* m_threads;
};

#endif

