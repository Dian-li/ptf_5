//
// Created by dian on 17-11-24.
//

#include <pthread.h>
#include <unistd.h>

#include "timeoutqueue.h"
#include "mptevent.h"
#include "tcpevent.h"
#include "transaction.h"

TimeoutQueue::TimeoutQueue() {
    m_node = NULL;
    size = 0;
}

TimeoutQueue::~TimeoutQueue()
{
    if(NULL != m_node)
    {
        TimeoutNode* node = m_node;
        while(node != NULL)
        {
            deTqueue(node);
            node = node->m_next;
        }
    }

}

void TimeoutQueue::enTqueue(TimeoutNode* node)
{
    m_tmutex.lock();
    size++;
    if(m_node==NULL){
        m_node = node;
        m_node->m_parent = NULL;
        m_node->m_next = NULL;
    }
    else
    {
        m_node->m_parent = node;
        m_node->m_name = m_node->m_time + rand();
        node->m_next = m_node;
        node->m_parent = NULL;
        m_node = node;
    }
    //printf("after added,node number is %d\n",size);
    m_tmutex.unlock();
}


void TimeoutQueue::loop()
{
    pthread_t pthread;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    int ret = pthread_create(&pthread,&attr,TimeoutQueue::lookuptime,(void*)this);
    printf("loop thread for check queue\n");
}



void* TimeoutQueue::lookuptime(void *arg)
{
    TimeoutQueue* timeoutQueue = (TimeoutQueue*)arg;
    TTCPEvent * delayEvent = new TTCPEvent();
    delayEvent->setType(ET_TIMEOUT);
    printf("loopuptime\n");
    while(true)
    {
        if(timeoutQueue->size>0)
        {
            time_t nowtime = time(NULL);
            TimeoutNode* node = timeoutQueue->m_node;
            while(node != NULL)
            {

                if(node->m_time >= nowtime)//timeout
                {
                    node->m_tTransation->pushEvent(delayEvent);
                    timeoutQueue->deTqueue(node);
                    printf("time delay!\n");
                }
                node = node->m_next;
            }
            //printf("time is %d\n",nowtime);
            //printf("node number is %d\n",timeoutQueue->size);
        }
        usleep(500000);//500000us = 500ms
    }
}

void TimeoutQueue::deTqueue(TimeoutNode* node)
{
	m_tmutex.lock();
    if(node != NULL)
    {
        TimeoutNode* nodepar = node->m_parent;
        TimeoutNode* nodechild = node->m_next;
        if(nodepar != NULL)
        {
        	nodepar->m_next = nodechild;
        }
        if(nodechild != NULL)
        {
            nodechild->m_parent = nodepar;
        }
        delete node;
        node = NULL;
        --size;
        //printf("after delete,node number is %d\n",size);
    }else{
    	printf("node is null!\n");
    }
    m_tmutex.unlock();
}

