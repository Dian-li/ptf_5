/**
*
* @file      cmdevent.h
* @brief     define cmdevent class, for internal event
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/08/30,  Initial version
*/

#ifndef _CMDEVENT_H
#define _CMDEVENT_H

#include "mptevent.h"

class TCmdStartEvent : public TMPTEvent
{
public:
    TCmdStartEvent(TScript* script, int tps, int total, int duration)
    {
        m_type = ET_CMD_START;
        m_direction = ED_RX;
        m_script = script;
        m_tps = tps;
        m_total = total;
        m_duration = duration;
    }
    ~TCmdStartEvent(){}
    const TScript* getScript() const {return m_script;}
    const TScript* getTps() const {return m_tps;}
    const TScript* getTotal() const {return m_total;}
    const TScript* getDuration() const {return m_duration;}
private:
    TScript* m_script;
    int m_tps;
    int m_total;
    int m_duration;
};

class TCmdStopEvent : public TMPTEvent
{
public:
    TCmdStopEvent(TScript* script)
    {
        m_type = ET_CMD_STOP;
        m_direction = ED_RX;
        m_script = script;
    }
    ~TCmdStopEvent(){}
private:
    TScript* m_script;
};

#endif
