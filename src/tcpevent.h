//
// Created by dian on 17-11-16.
//

#ifndef PTF_5_TCPEVENT_H
#define PTF_5_TCPEVENT_H

#include "mptevent.h"
class TTCPEvent:public TMPTEvent
{
public:
    TTCPEvent();
    TTCPEvent(TEventType type,TEventDirection direction);
    ~TTCPEvent();
    void setData(char* data);
    void setType(TEventType type);
    char* getData();
    void setLength(int len);
    int getLength();

private:
    char* m_data;
    int m_length;

};
#endif //PTF_5_TCPEVENT_H
