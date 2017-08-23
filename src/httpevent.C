/**
*
* @file      httpevent.h
* @brief     define httpevent class, for internal event
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <event2/http.h>
#include <event2/http_struct.h>
#include "httpevent.h"
#include "mptconst.h"

THttpEvent::THttpEvent(TEventDirection direct, struct evhttp_request* pMsg)
{
    m_type = ET_HTTP;
    m_direction = direct;
    if(NULL != pMsg)
    {
        m_httpMsg = pMsg;
    }
    else
    {
        //m_httpMsg = evhttp_request_new(); //TODO
    }
}

THttpEvent::~THttpEvent()
{
    if(likely(NULL != m_httpMsg))
    {
        evhttp_request_free(m_httpMsg);
        m_httpMsg = NULL;
    }
}

bool THttpEvent::isRequest()
{
    return (EVHTTP_REQUEST == m_httpMsg->kind ? true : false);
}

enum evhttp_cmd_type THttpEvent::getCmdType()
{
    return m_httpMsg->type;
}

