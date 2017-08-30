/**
*
* @file      step.h
* @brief     define script's send class
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _STEPSEND_H
#define _STEPSEND_H

class TStep;
class TMPTEvent;
class TStepSend : public TStep
{
public:
    TStepSend();
    ~TStepSend();
    int run(TMPTEvent* pEvent);
private:
    const char* m_ip;
    unsigned short m_port;
    int  m_wait; //seconds
};

#endif

