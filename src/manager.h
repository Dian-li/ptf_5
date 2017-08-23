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
    int startHttpServer();;

private:
    struct event_base *m_base;
    struct evhttp *m_httpd;
    TMptThreadPool m_threadPool;
};

#endif
