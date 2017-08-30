/**
*
* @file      manager.h
* @brief     define manager module, main thread call it
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _MANAGER_H
#define _MANAGER_H

#include <event2/event.h>
#include <event2/http.h>
#include <vector>
using std::vector;

#include "mptthreadpool.h"

class TManager
{
public:
    TManager();
    ~TManager();
    int init();
    void loop();
    void processHttpReq(struct evhttp_request *req);

private:
    int startHttpServer();
    /**
    * @brief choose worker thread to run task
    * @param workers output worker thread index
    * @return <0 indicates error, 0 indicates all workers, >0 worker count, index save in workers vector
    */
    int chooseWorkers(vector<int>& workers);

private:
    struct event_base *m_base;
    struct evhttp *m_httpd;
    TMptThreadPool m_threadPool;
};

#endif
