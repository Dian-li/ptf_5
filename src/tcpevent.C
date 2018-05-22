//
// Created by dian on 17-11-16.
//
#include "tcpevent.h"
#include <stdlib.h>
#include "stdio.h"

TTCPEvent::TTCPEvent() {}

TTCPEvent::TTCPEvent(TEventType type, TEventDirection direction)
{
    m_type = type;
    m_direction = direction;
}

TTCPEvent::~TTCPEvent()
{
    if(m_data!= NULL)
    {
        free(m_data);
        m_data = NULL;
    }
}

void TTCPEvent::setData(char *data)
{
    m_data = (char*)malloc(sizeof(data));
    m_data = data;
}

char* TTCPEvent::getData()
{
    return m_data;
}

void TTCPEvent::setType(TEventType type)
{
    m_type = type;
}

void TTCPEvent::setLength(int len){
	m_length = len;
}

int TTCPEvent::getLength(){
	return m_length;
}



