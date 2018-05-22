/**
*
* @file      tcpstep.C
* @brief     define tcp step function
* @author    wangqihua
* @History
*    1.0.0: wangqihua,  2017/08/07,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "tcpstep.h"
#include "mptconst.h"
#include "tcpevent.h"
#include "timeoutqueue.h"

#define CSOCKETPOOLSIZE 2
TTCPStep::TTCPStep()
{
   m_port = 0;
}

TTCPStep::~TTCPStep()
{
}

bool TTCPStep::init(xmlNodePtr pNode)
{
   xmlChar* pValue = NULL;
    printf("ready init\n");
   bool ret = TStep::init(pNode);
   if(unlikely(!ret)) return ret;
   m_protocol = P_TCP;
   xmlNodePtr pChildNode = pNode->xmlChildrenNode;
   while(NULL != pChildNode)
   {
       if (xmlNodeIsText(pChildNode) || XML_CDATA_SECTION_NODE == pChildNode->type) {
           pChildNode = pChildNode->next;
           continue;
       }
       char * node_name = (char *)pChildNode->name;
       char * node_value = (char *)xmlNodeGetContent(pChildNode);
       //printf("[%s:",node_name);
       //printf("%s]\n",node_value);
      if(string(node_name)=="wait")
      {
         if(NULL != node_value){
            //printf("time delay:%s s\n",node_value);
             m_timer = atoi((const char*)node_value);
         }
      }
      else if(string(node_name)=="ip")
      {
         if(NULL != node_value)
         {
            m_ip = (const char*)node_value;
         }
         else
         {
            printf("TTCPStep init() get destination ip address failed !\n");
            return false;
         }
      }
      else if(string(node_name)=="port")
      {
         if(NULL != node_value)
         {
            m_port = atoi((const char*)node_value);
         }
         else
         {
            printf("TTCPStep init() get destination port failed !\n");
            return false;
         }
      }
       pChildNode = pChildNode->next;
   }
    if(m_type == ST_SEND)
    {
        m_tmptEvent = new TTCPEvent(ET_TCPDATA,ED_RX);
    }
    else if(m_type == ST_RECV)
    {
        m_tmptEvent = new TTCPEvent(ET_TCPDATA,ED_TX);
    }
    return true;
}


TStepResult TTCPStep::send(TMPTEvent * pEvent)
{
   if(ST_RECV == m_type)
   {
      return SRST_ERREND;
   }
    pthread_t thread_id = pthread_self();
    cps_map_mutex.lock();
    //printf("thread lock is error\n");
    m_cSocket = (CSocket*)cps_map[thread_id];
    cps_map_mutex.unlock();
    //printf("thread lock is error2\n");
    int fd = m_cSocket->getSocketFd();
    m_cSocket->sendMsg(m_data.content,fd);
    std::cout<<"[send client]"<<m_data.content<<std::endl;

    return SRST_SUCC;
}

TStepResult TTCPStep::recv(TMPTEvent * pEvent)
{
   if(ST_SEND == m_type)
   {
      return SRST_ERREND;
   }
    TEventType tEventType = pEvent->type();
    if(tEventType == ET_TIMEOUT)
    {
        return SRST_ERREND;
    }
    else if(tEventType == ET_TCPDATA)
    {
       // char * recv_data = dynamic_cast<TTCPEvent*>(pEvent)->getData();
    	int recv_len = dynamic_cast<TTCPEvent*>(pEvent)->getLength();
        TTCPStep* sendStep = (TTCPStep*)this->m_parent;
        pthread_t thread_id = pthread_self();
        cps_map_mutex.lock();
        m_cSocket = (CSocket*)cps_map[thread_id];
        TimeoutNode* pnode = m_cSocket->getTTransation()->getTimeoutNode();
        //m_cSocket->getTimeoutQueue()->deTqueue(pnode);    //删除节点
        cps_map_mutex.unlock();
        m_cSocket = sendStep->m_cSocket;
        int fd = m_cSocket->getSocketFd();
        string recvMsg = m_data.content;
        int msg_len = recvMsg.length();
        //std::cout<<"msg len is"<<msg_len<<std::endl;
        //std::cout<<"recv_len is"<<recv_len<<std::endl;
        if(msg_len==recv_len){
        	printf("success!\n");
            return SRST_SUCC;
        }
        else{
            return SRST_ERREND;
        }
    }
    else
    {
        return SRST_ERREND;
    }

}

void TTCPStep::supsend()
{
    int overtime = this->m_timer;
    //printf("wait for %d seconds\n",overtime);
}

void TTCPStep::suprecv()
{
    int overtime = this->m_timer+ time(NULL);//超时时间
    pthread_t thread_id = pthread_self();
    cps_map_mutex.lock();
    m_cSocket = (CSocket*)cps_map[thread_id];
    cps_map_mutex.unlock();
    int fd = m_cSocket->getSocketFd();
    m_cSocket->getTTransation()->setOverTime(overtime);
    TimeoutNode* node = new TimeoutNode();
    node->m_tTransation = m_cSocket->getTTransation();
    node->m_time = overtime;
    m_cSocket->getTTransation()->setTimeoutNode(node);
    m_cSocket->getTimeoutQueue()->enTqueue(node);
    if(node == NULL){
    	printf("what fuck?!\n");
    }
    //printf("TTransation set timeout is %d seconds\n",overtime);
}
