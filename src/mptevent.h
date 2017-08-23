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
    ET_SQLRESULT,
    ET_TIMEOUT
};

enum TEventDirection
{
    ED_RX,
    EX_TX
};

class TMTPEvent
{
public:
    TMTPEvent(){}
    virtual ~TMTPEvent(){}
    TEventType type() const {return m_type;}
    TEventDirection direction() const {return m_direction;}
protected:
    TEventType  m_type;
    TEventDirection m_direction;
};

#endif