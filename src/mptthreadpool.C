/**
*
* @file      mptthreadpool.C
* @brief     define MPT worker thread pool
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/08/08,  Initial version
*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "mptconst.h"
#include "mptthreadpool.h"

TMptThreadPool::TMptThreadPool()
{
    m_nCount = 0;
    m_threads = NULL;
}

TMptThreadPool::~TMptThreadPool()
{
    if(likely(NULL != m_threads))
    {
        delete []m_threads;
        m_threads = NULL;
        m_nCount = 0;
    }
}

int TMptThreadPool::init(int nThread)
{
    int ret = 0;
    if(unlikely(NULL != m_threads))
    {
        return -1;
    }
    m_nCount = nThread;
    m_threads = (TThreadData*)calloc(nThread, sizeof(TThreadData));
    if(unlikely(NULL == m_threads))
    {
        perror("Can't allocate thread descriptors");
        return -2;
    }
    for (int i = 0; i < m_nCount; ++i)
    {
        int fds[2];
        if (pipe(fds))
        {
            perror("Can't create pipes");
            return -3;
        }

        m_threads[i].m_readCmdEventFd = fds[0];
        m_threads[i].m_writeCmdEventFd = fds[1];

        setupThread(&m_threads[i]);
    }
    for (int i = 0; i < m_nCount; ++i)
    {
        ret = createThread(workerFunc, &m_threads[i]);
        if(unlikely(ret)) return ret;
    }
    return ret;
}

bool TMptThreadPool::order(TMPTEvent * pCmd, int id)
{
    bool ret = false;
    if(unlikely(NULL == pCmd)) return ret;
    TEventType cmdType = pCmd->type();
    char buf[1] = {THR_EVENT};
    if(likely(id >= 0 && id < m_nCount))
    {
        ret = m_threads[id].m_queue.enqueue(pCmd);
        if(likely(ret))
        {
            if(likely(write(m_threads[id].m_readCmdEventFd, buf, 1) == 1))
            {
                ret = true;
            }
            else
            {
                perror("Fatal error, notify to worker failed.");
            }
        }
    }
    else
    {
        for(id=0; id < m_nCount; ++id)
        {
            ret = m_threads[id].m_queue.enqueue(pCmd);
            if(likely(ret))
            {
                if(likely(write(m_threads[id].m_readCmdEventFd, buf, 1) == 1))
                {
                    ret = true;
                }
                else
                {
                    perror("Fatal error, notify to worker failed.");
                }
            }
        }
    }
    return ret;
}

void* TMptThreadPool::workerFunc(void * arg)
{
    TThreadData* p = (TThreadData*)arg;
    event_base_dispatch(p->m_base);
    return NULL;
}

void TMptThreadPool::threadEventProcess(int fd, short which, void *arg)
{
    TThreadData *me = (TThreadData*)arg;
    char buf[1];
    unsigned int timeout_fd;

    if (read(fd, buf, 1) != 1)
    {
        return;
    }

    switch (buf[0])
    {
    case THR_EVENT:
        {
            TMPTEvent* pEvent = me->m_queue.dequeue();
            TEventType type = pEvent->type();
            switch(type)
            {
            case ET_CMD_START:
                
            }
        }
    //TODO
    default:
        break;
    }
    return;
}

int TMptThreadPool::setupThread(TThreadData * data)
{
    data->m_base = event_base_new();
    if(unlikely(NULL == data->m_base))
    {
        perror("Can't allocate base event for thread.");
        return -2;
    }
    event_set(&data->m_notifyEvent,
                data->m_readCmdEventFd,
                EV_READ | EV_PERSIST,
                threadEventProcess,
                data);
    event_base_set(data->m_base, &data->m_notifyEvent);

    if (unlikely(event_add(&data->m_notifyEvent, 0) == -1))
    {
        perror("Can't add event\n");
        return -3;
    }
    return 0;
}

int TMptThreadPool::createThread(void *(*func)(void *), TThreadData *arg)
{
    pthread_attr_t  attr;
    int             ret;

    pthread_attr_init(&attr);

    if (unlikely((ret = pthread_create(&((TThreadData*)arg)->m_threadID, &attr, func, arg)) != 0))
    {
        fprintf(stderr, "Can't create thread: %s\n", strerror(ret));
    }
    return ret;
}

