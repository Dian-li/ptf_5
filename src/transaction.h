/**
*
* @file      transaction.h
* @brief     define transaction base class, store the data and context while running
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _TRANSACTION_H
#define _TRANSACTION_H

#include "ringbuff.h"
#include "timeoutqueue.h"
//#include "socketpool.h"

class TMPTEvent;
class TScript;
class TStep;
struct event_base;
class TTransation
{
public:
    TTransation(TScript* pScript, struct event_base* base);
    ~TTransation();
    bool pushEvent(TMPTEvent* pEvent);
    int onEvent();
    void setOverTime(int overtime);
    int getOverTime();
    void setTimeoutNode(TimeoutNode* node);
    TimeoutNode* getTimeoutNode();
    struct event_base *m_base;
private:
    TRingbuffer<TMPTEvent> *m_pMsgQueue;
    TMPTEvent* m_pCurEvent;
    TScript* m_pScript;
    TStep*   m_curStep;
    bool isSend;
    int times;

    TimeoutNode* m_node;
};

#endif

