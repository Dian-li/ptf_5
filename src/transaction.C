/**
*
* @file      transaction.C
* @brief     define transaction base class, store the data and context while running
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "transaction.h"
#include "mptevent.h"
#include "script.h"
#include "step.h"
#include "mptconst.h"

using namespace std;
TTransation::TTransation(TScript* pScript, struct event_base* base)
{
    assert(NULL != pScript);
    m_pMsgQueue = new TRingbuffer<TMPTEvent>(512);
    assert(NULL != m_pMsgQueue);
    m_base = base;
    isSend = true;
    m_pScript = pScript;
    m_pScript->setTTransation(this);
    m_curStep = m_pScript->enter();
}

TTransation::~TTransation()
{
    if(likely(NULL != m_pMsgQueue))
    {
        delete m_pMsgQueue;
        m_pMsgQueue = NULL;
    }
}

bool TTransation::pushEvent(TMPTEvent * pEvent)
{
    if(unlikely(NULL == pEvent)) return false;
    bool ret = m_pMsgQueue->enqueue(pEvent);
    return ret;
}

int TTransation::onEvent()
{
    TStepResult ret;
    TMPTEvent* pEvent = NULL;
    do
    {
        if(m_pMsgQueue->size() > 0)
        {
            pEvent = m_pMsgQueue->dequeue();
        }
        else
        {
            //printf("event is null!\n");
            pEvent = NULL;
        }
        ret = SRST_SUCC;
        if(unlikely(NULL == pEvent))//pEvent is null
        {
            if(isSend)//send
            {
                isSend = false;
            }
            else//recv
            {
                continue;//becase recv event can not be null
            }
        }
        else//has a recv event
        {
            printf("have a recv event\n");
            isSend = true;
        }

        ret = m_curStep->run(pEvent);
        switch(ret)
        {
            case SRST_ERREND:
                m_curStep = NULL;
                break;
            case SRST_SUCC:
                m_curStep = m_curStep->next();
                break;
            case SRST_SUBRETURN:
                m_curStep = m_curStep->parent();
                break;
            case SRST_NEXTEVENT:
                m_curStep = m_curStep->next();
                return ret;
        }
        if(unlikely(NULL == m_curStep))
        {
        	printf("exit!\n");
            m_pScript->exit();
            return ret;
        }
    }while(true);
    return ret;
}

void TTransation::setOverTime(int overtime)
{
    times = overtime;
}

int TTransation::getOverTime()
{
    return times;
}

TimeoutNode* TTransation::getTimeoutNode()
{
    return m_node;
}

void TTransation::setTimeoutNode(TimeoutNode *node)
{
    m_node = node;
}

