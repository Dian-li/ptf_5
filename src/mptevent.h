/**
*
* @file      mptevent.h
* @brief     define mptevent class, for internal event
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _MTPEVENT_H
#define _MTPEVENT_H

enum TEventType
{
    ET_HTTP,
    ET_FTPCMD,
    ET_FTPDATA,
    ET_TCPDATA,
    ET_SQLRESULT,
    ET_TIMEOUT,
    ET_ERROR,
    ET_CMD_START,
    ET_CMD_STOP,
	ET_UNDEFINED
};

enum TEventDirection
{
    ED_RX,
    ED_TX
};

class TMPTEvent
{
public:
    TMPTEvent(){}
    TMPTEvent(const TMPTEvent& e)
    {
        m_type = e.m_type;
        m_direction = e.m_direction;
    }
    virtual ~TMPTEvent(){}
    TEventType type() const {return m_type;}
    TEventDirection direction() const {return m_direction;}
protected:
    TEventType  m_type;
    TEventDirection m_direction;
};

#endif
