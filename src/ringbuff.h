/**
*
* @file      ringbuff.h
* @brief     define ringbuff class, high performance queue
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _RINGBUFF_H
#define _RINGBUFF_H

#include <assert.h>
#include "atomic.h"

#ifndef NULL
#define NULL (0)
#endif

template<class T>
class TRingbuffer
{
public:
    TRingbuffer(unsigned int size, bool sp=true, bool sc=true)
    {
        assert(size > 1);
        m_ring = new T*[size];
        m_size = size;
        m_head = 0;
        m_tail = 0;
        m_sp = sp;
        m_sc = sc;
    }
    ~TRingbuffer()
    {
        if(NULL != m_ring)
        {
            T* p = dequeue();
            while(NULL != p)
            {
                delete p;
                p = dequeue();
            }
        }
    }
    bool enqueue(T* item)
    {
        unsigned int tailOld, tailNew;
        do
        {
            tailOld = m_tail;
            tailNew = (tailOld+1)%m_size;
            if(tailNew == m_head)
            {
                return false;
            }
            if(m_sp)
            {
                m_ring[tailOld] = item;
                m_tail = tailNew;
                return true;
            }
            else
            {
                if(!CAS(&(m_ring[tailOld]), NULL, item))
                    continue;
            }
        }while(!CAS(&m_tail, tailOld, tailNew));
        return true;
    }
    T* dequeue()
    {
        unsigned int headOld, headNew;
        T* p = NULL;
        do
        {
            headOld = m_head;
            p = m_ring[headOld];
            if(headOld == m_tail)
            {
                return NULL;
            }
            if(NULL == p) continue;
            headNew = (headOld+1)%m_size;
            if(m_sc)
            {
                m_ring[headOld] = NULL;
                m_head = headNew;
                return p;
            }
            else
            {
                if(!CAS(&(m_ring[headOld]), p, NULL))
                    continue;
            }
        }while(!CAS(&m_head, headOld, headNew));
        return p;
    }
    int size()
    {
        unsigned int h, t;
        h = m_head;
        t = m_tail;
        return (h < t ? t-h : m_size-h+t);
    }

private:
    T **m_ring;
    unsigned int m_size;
    volatile unsigned int m_head;
    volatile unsigned int m_tail;
    bool m_sp;
    bool m_sc;
};

#endif

