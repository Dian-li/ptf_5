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
#include<iostream>

#include "mptconst.h"
#include "mptthreadpool.h"
#include "socketpool.h"

std::unordered_map<pthread_t, Connection*> cps_map;
std::mutex cps_map_mutex;
TThreadData::TThreadData() {
  m_queue = new TRingbuffer<TMPTEvent>(256);
  assert(NULL != m_queue);
  m_httpbase = event_base_new();
}

TThreadData::~TThreadData() {
  if (likely(NULL != m_base)) {
    event_base_free(m_base);
    m_base = NULL;
  }
  if (likely(NULL != m_queue)) {
    delete m_queue;
    m_queue = NULL;
  }
  if (likely(NULL != m_httpbase)) {
    event_base_free(m_httpbase);
    m_httpbase = NULL;
  }
}

TMptThreadPool::TMptThreadPool() {
  m_nCount = 0;
  m_threads = NULL;
}

TMptThreadPool::~TMptThreadPool() {
  if (likely(NULL != m_threads)) {
    delete[] m_threads;
    m_threads = NULL;
    m_nCount = 0;
  }
}

int TMptThreadPool::init(int nThread) {
  int ret = 0;
  if (unlikely(NULL != m_threads)) {
    return -1;
  }
  m_nCount = nThread;
  m_threads = new TThreadData[nThread];
  if (unlikely(NULL == m_threads)) {
    perror("Can't allocate thread descriptors");
    return -2;
  }
  for (int i = 0; i < m_nCount; ++i) {
    int fds[2];     //  管道，0-读，1-写；
    if (pipe(fds)) {
      perror("Can't create pipes");
      return -3;
    }
    m_threads[i].m_readCmdEventFd = fds[0];
    m_threads[i].m_writeCmdEventFd = fds[1];
    setupThread(&m_threads[i]);
  }
  for (int i = 0; i < m_nCount; ++i) {
    ret = createThread(workerFunc, &m_threads[i]);
    if (unlikely(ret))
      return ret;
  }
  return ret;
}

bool TMptThreadPool::order(TMPTEvent * pCmd, int id) {
  bool ret = false;
  if (unlikely(NULL == pCmd))
    return ret;
  TEventType cmdType = pCmd->type();
  char buf[1] = { THR_EVENT };
  if (likely(id >= 0 && id < m_nCount)) {
    ret = m_threads[id].m_queue->enqueue(pCmd);
    if (likely(ret)) {
      if (likely(write(m_threads[id].m_writeCmdEventFd, buf, 1) == 1)) {
        ret = true;
      } else {
        perror("Fatal error, notify to worker failed.");
      }
    }
  }

  return ret;
}

void* TMptThreadPool::workerFunc(void * arg) {
  TThreadData* p = (TThreadData*) arg;
  event_base_dispatch(p->m_base);
  return NULL;
}

void TMptThreadPool::threadEventProcess(int fd, short which, void *arg)  //  文件描述符、事件类型、参数
                                        {
  TThreadData *me = (TThreadData*) arg;
  char buf[1];
  unsigned int timeout_fd;

  if (read(fd, buf, 1) != 1) {
    return;
  }

  switch (buf[0]) {
    case THR_EVENT: {
      TMPTEvent* pEvent = me->m_queue->dequeue();
      TEventType type = pEvent->type();
      switch (type) {
        case ET_CMD_START:
          startTask(dynamic_cast<TCmdStartEvent*>(pEvent), me);
          break;
        case ET_CMD_STOP:
          stopTask(dynamic_cast<TCmdStopEvent*>(pEvent), me);
          break;
        default:
          ;
      }
      delete pEvent;
      pEvent = NULL;
    }
      //TODO
    default:
      break;
  }
  return;
}

void TMptThreadPool::time_cb(evutil_socket_t fd, short event, void *arg) {
  TThreadData* pThrData = (TThreadData*) arg;
  printf("ready send to redis\n");
  pThrData->m_taskInfo.m_script->m_send_st2redis();
  bool ret = runTask(pThrData);
  if (likely(ret)) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    evtimer_add(&pThrData->m_timerEvent, &tv);
  } else {
    evtimer_del(&pThrData->m_timerEvent);
  }
  return;
}

int TMptThreadPool::setupThread(TThreadData * data) {
  data->m_base = event_base_new();
  if (unlikely(NULL == data->m_base)) {
    perror("Can't allocate base event for thread.");
    return -2;
  }
  event_set(&data->m_notifyEvent, data->m_readCmdEventFd,  //  文件描述符或者信号，对于定时事件，设为-1
            EV_READ | EV_PERSIST,   //  事件类型
            threadEventProcess,     //  回调函数
            data);                  //  参数
  event_base_set(data->m_base, &data->m_notifyEvent);
  evtimer_set(&data->m_timerEvent, TMptThreadPool::time_cb, data);
  event_base_set(data->m_base, &data->m_timerEvent);

  if (unlikely(event_add(&data->m_notifyEvent, 0) == -1)) {
    perror("Can't add event\n");
    return -3;
  }
  return 0;
}

int TMptThreadPool::createThread(void *(*func)(void *), TThreadData *arg) {
  pthread_attr_t attr;
  int ret;

  pthread_attr_init(&attr);

  if (unlikely((ret = pthread_create(&((TThreadData* )arg)->m_threadID, &attr, func, arg)) != 0)) {
    fprintf(stderr, "Can't create thread: %s\n", strerror(ret));
  }
  return ret;
}

bool TMptThreadPool::startTask(TCmdStartEvent * pCmd, TThreadData* pThrData) {
  pThrData->m_taskInfo.m_script = pCmd->getScript();
  pThrData->m_taskInfo.m_tps = pCmd->getTps();
  pThrData->m_taskInfo.m_total = pCmd->getTotal();
  pThrData->m_taskInfo.m_duration = pCmd->getDuration();
  pThrData->m_taskInfo.m_nCount = 0;
  pThrData->m_taskInfo.m_nSecs = 0;
  bool ret = runTask(pThrData);
  if (likely(ret)) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    evtimer_add(&pThrData->m_timerEvent, &tv);
  }
  return ret;
}

bool TMptThreadPool::stopTask(TCmdStopEvent * pCmd, TThreadData* pThrData) {
  TTaskInfo& ti = pThrData->m_taskInfo;
  bool ret = false;
  if (ti.m_script == pCmd->getScript()) {
    ti.m_stopFlag = true;
    ret = true;
  }
  return ret;
}

bool TMptThreadPool::runTask(TThreadData * pThrData) {
  TTaskInfo& ti = pThrData->m_taskInfo;
  int nNewTask = ti.m_tps;
  bool ret = true;
  if (ti.m_total > 0) {
    printf("total task is %d\n", ti.m_total);
    nNewTask = (ti.m_nCount + nNewTask > ti.m_total ? ti.m_total - ti.m_nCount : nNewTask);
    if (unlikely(nNewTask <= 0)) {
      ret = false;
    }
  } else if (ti.m_duration > 0) {
    if (unlikely(ti.m_nSecs + 1 > ti.m_duration)) {
      ret = false;
    }
  }
  if (likely(ret)) {
    ti.m_nCount += nNewTask;
    ++ti.m_duration;
    //TODO
    for (int i = 0; i < nNewTask; ++i) {
      printf("task -%d- ,total task %d\n", i, ti.m_nCount);
      TTransation* pTrans = new TTransation(ti.m_script, pThrData->m_base);
      if (cps_map.size() > 0) {
        if (ti.m_script->m_protocol == "TCP") {
          cps_map_mutex.lock();
          CSocket * cSocket = (CSocket *) cps_map[pThrData->m_threadID];
          cps_map_mutex.unlock();
          std::cout << "[thread" << pThrData->m_threadID << "]:CSocket is " << cSocket << std::endl;
          std::cout << "[thread" << pThrData->m_threadID << "]:Transation is " << pTrans << std::endl;
          if (NULL != cSocket) {
            cSocket->setTransation(pTrans);
          }
        } else if (ti.m_script->m_protocol == "HTTP") {
          cps_map_mutex.lock();
          CHttpConnection* cHttpConn = (CHttpConnection*) cps_map[pThrData->m_threadID];
          cps_map_mutex.unlock();
          std::cout << "[thread" << pThrData->m_threadID << "]:cHttpConn is " << cHttpConn << std::endl;
          std::cout << "[thread" << pThrData->m_threadID << "]:Transation is " << pTrans << std::endl;
          if (NULL != cHttpConn) {
            cHttpConn->m_tTransation = pTrans;
          }
        }
      }
      if (unlikely(NULL == pTrans))
        break;
      pTrans->onEvent();
    }
  }
  return ret;
}

void TMptThreadPool::initConnect(int i, TScript *tScript, CSocket *cSocket) {
  string connStr = tScript->getConnStr();
  if (connStr.size() > 0) {
    cps_map[m_threads[i].m_threadID] = cSocket;                 //put in the map
    std::cout << "[thread " << m_threads[i].m_threadID << "]:CSocket=" << cSocket << std::endl;
  }
}

void TMptThreadPool::initHttpConnect(int i) {
  struct event_base* base = m_threads[i].m_httpbase;
  CHttpConnection* cHttpConn = new CHttpConnection(base);
  cps_map[m_threads[i].m_threadID] = cHttpConn;
}

