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

class TMPTEvent;
class TScript;
class TStep;
struct event_base;
class TTransation
{
public:
    TTransation(TScript* pScript);
    ~TTransation();
    bool pushEvent(TMPTEvent* pEvent);
    int onEvent();
private:
    TRingbuffer<TMPTEvent> *m_pMsgQueue;
    TMPTEvent* m_pCurEvent;
    struct event_base *m_base;
    TScript* m_pScript;
    TStep*   m_curStep;
};

#endif

