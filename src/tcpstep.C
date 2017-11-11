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
    printf("ready init");
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
       printf("[%s:",node_name);
       printf("%s]\n",node_value);
      if(string(node_name)=="wait")
      {
         if(NULL != node_value){
            printf("time delay:%s ms\n",node_value);
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
    std::stringstream ss;
    ss<<m_ip;
    ss<<":";
    ss<<m_port;
    m_connStr = ss.str();
    printf("%s\n",m_connStr.c_str());
    return true;
}


TStepResult TTCPStep::send(TMPTEvent * pEvent)
{
   if(ST_RECV == m_type)
   {
      return SRST_ERREND;
   }
    csocket_map_mutex.lock();
    if(csocket_map.find(m_connStr)==csocket_map.end())
    {
        const char* connStr = m_connStr.data();
        printf("no such a socket connect pool,now create a new one:%s\n",connStr);
        csocket_map[m_connStr] = new CSocketPool(CSOCKETPOOLSIZE,connStr);
        printf("connect pool size is %d\n",csocket_map.size());
    }
    csocket_map_mutex.unlock();
    m_conn_socket = csocket_map[m_connStr];
    m_cSocket = m_conn_socket->getSocket();
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
    TTCPStep* sendStep = (TTCPStep*)this->m_parent;
    m_conn_socket = sendStep->m_conn_socket;
    m_cSocket = sendStep->m_cSocket;
    int fd = m_cSocket->getSocketFd();
    string recvMsg = m_data.content;
    string result = m_cSocket->recvMsg(fd);
    std::cout<<"[recv got]"<<recvMsg<<std::endl;
    std::cout<<"[recv except]"<<result<<std::endl;
    m_conn_socket->backSocket(m_cSocket);
    if(recvMsg==result){
        return SRST_SUCC;
    }else{
        return SRST_ERREND;
    }

}
