/**
*
* @file      httpevent.h
* @brief     define httpevent class, for internal event
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _HTTPEVENT_H
#define _HTTPEVENT_H

#include "mptevent.h"

struct evhttp_request;
enum evhttp_cmd_type;
class THttpEvent : public TMTPEvent
{
public:
    THttpEvent(TEventDirection direct, struct evhttp_request* pMsg);
    ~THttpEvent();
    bool isRequest();
    enum evhttp_cmd_type getCmdType();
private:
    struct evhttp_request* m_httpMsg;
};

#endif

