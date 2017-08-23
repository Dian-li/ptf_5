/**
*
* @file      step.h
* @brief     define script step class, script run with it
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _STEP_H
#define _STEP_H

enum TStepType
{
    ST_SEND,
    ST_RECV,
    ST_CHECK
};

enum TStepResult
{
    SRST_ERREND = -1,
    SRST_SUCC = 0,
    SRST_SUBRETURN,
    SRST_NEXTEVENT
};

const char* g_sStepTypeStr[]=
{
    "send",
    "recv",
    "check"
};

#define STEP_TYPE_STR(x) (x>=0 && x<sizeof(g_sStepTypeStr)/sizeof(g_sStepTypeStr[0]) ? g_sStepTypeStr[x] : "unknow")

class TStep
{
public:
    TStep()
    {
        m_next = NULL;
        m_child = NULL;
        m_parent = NULL;
    }
    ~TStep()
    {
        if(NULL != m_next)
        {
            delete m_next;
            m_next = NULL;
        }
        if(NULL != m_child)
        {
            delete m_child;
            m_child = NULL;
        }
    }
    TStep* next() const
    {
        if(NULL != m_child)
            return m_child;
        else if(NULL != m_next)
            return m_next;
        else
            return m_parent;
    }
    TStep* parent() const
    {
        return m_parent;
    }
    virtual TStepResult run(TMTPEvent* pEvent)=0;
private:
    TStepType m_type;
    TStep* m_next;
    TStep* m_child;
    TStep* m_parent;
};

#endif

