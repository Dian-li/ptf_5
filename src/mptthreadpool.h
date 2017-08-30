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
#include "cmdevent.h"

#define THR_EVENT 'E'

struct TTaskInfo
{
    TScript* m_script;
    int m_tps;
    int m_total;
    int m_duration;
    int m_nCount;
    int m_nSecs;
    bool m_stopFlag;

    TTaskInfo()
    {
        m_script = NULL;
        m_tps = 0;
        m_total = 0;
        m_duration = 0;
        m_nCount = 0;
        m_nSecs = 0;
        m_stopFlag = false;
    }
};

class TThreadData
{
public:
    TThreadData();
    ~TThreadData();
    
    pthread_t  m_threadID;
    struct event_base *m_base;
    struct event m_notifyEvent;
    int m_writeCmdEventFd;
    int m_readCmdEventFd;
    TRingbuffer<TMPTEvent> *m_queue;
    struct event m_timerEvent;
    //TODO multi task
    TTaskInfo m_taskInfo;
};

class TMptThreadPool
{
public:
    TMptThreadPool();
    ~TMptThreadPool();
    int init(int nThread);
    bool order(TMPTEvent* pCmd, int id=-1);

private:
    static void* workerFunc(void* arg);
    static void threadEventProcess(int fd, short which, void *arg);
    static void time_cb(evutil_socket_t fd, short event, void *arg);
    int setupThread(TThreadData* data);
    int createThread(void *(*func)(void *), TThreadData *arg);
    static bool startTask(TCmdStartEvent* pCmd, TThreadData* pThrData);
    static bool stopTask(TCmdStopEvent* pCmd, TThreadData* pThrData);
    static bool runTask(TThreadData* pThrData);

private:
    int m_nCount;
    TThreadData* m_threads;
};

#endif

