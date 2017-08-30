/**
*
* @file      transaction.C
* @brief     define transaction base class, store the data and context while running
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <assert.h>

#include "transaction.h"
#include "mptevent.h"
#include "script.h"
#include "step.h"
#include "mptconst.h"

TTransation::TTransation(TScript* pScript, struct event_base* base)
{
    assert(NULL != pScript);
    //m_pMsgQueue = new TRingbuffer<TMPTEvent>(256);
    //assert(NULL != m_pMsgQueue);
    m_base = base;
    m_pScript = pScript;
    m_curStep = m_pScript->enter();
}

TTransation::~TTransation()
{
    //if(likely(NULL != m_pMsgQueue))
    //{
    //    delete m_pMsgQueue;
    //    m_pMsgQueue = NULL;
    //}
}

/*bool TTransation::pushEvent(TMPTEvent * pEvent)
{
    if(unlikely(NULL == pEvent)) return false;
    bool ret = m_pMsgQueue->enqueue(pEvent);
    return ret;
}
*/

int TTransation::onEvent()
{
    TMPTEvent* pEvent = NULL;
    //m_pMsgQueue->dequeue();
    TStepResult ret = SRST_SUCC;
    //if(unlikely(NULL == pEvent)) return ret;
    do
    {
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
            m_pScript->exit();
            return ret;
        }
    }while(true);
    return ret;
}

