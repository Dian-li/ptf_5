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

#include "tcpstep.h"

TTCPStep::TTCPStep()
{
   m_port = 0;
}

TTCPStep::~TTCPStep()
{
    if(NULL != m_data.content)
    {
        delete []m_data.content;
        m_data.content = NULL;
        m_data.length = 0;
    }
}

bool TTCPStep::init(xmlNodePtr pNode)
{
   xmlChar* pValue = NULL;
   int      i = 0;
   int      length = 0;
   
   if(xmlStrcmp(pNode->name, BAD_CAST("send")))
   {
      m_type = ST_SEND;
   }
   else if(xmlStrcmp(pNode->name, BAD_CAST("recv")))
   {
      m_type = ST_RECV;
   }
   else
   {
      printf("TTCPStep init() find unknow step type(%s)", (char *)pNode->name);
      return false;
   }
   
   m_protocol = P_TCP;
   
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"ip");
   if(NULL == pValue)
   {
      printf("TTCPStep init() get destination ip address failed !");
      return false;
   }
   else
   {
      m_ip = (const char*)pValue;
   }
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"port");
   if(NULL == pValue)
   {
      printf("TTCPStep init() get destination port failed !");
      return false;
   }
   else
   {
      m_port = atoi((const char*)pValue);
   }
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"CDATA");
   if(NULL == pValue)
   {
      printf("TTCPStep init() get CDATA failed !");
      return false;
   }
   length = strlen((const char*)pValue);
   m_data.length = 0;
   m_data.content = new char[length];
   for(i = 0; i < length; i++)
   {
      if('\\' == pValue[i] && ('x' == pValue[i + 1] || 'X' == pValue[i + 1]))
      {
         m_data.content[m_data.length++] = (unsigned char)pValue[i + 2] * 16 + (unsigned char)pValue[i + 3];
         i += 3;
      }
      else
      {
         m_data.content[m_data.length++] = pValue[i];
      }
   }
}


TStepResult TTCPStep::send(TMTPEvent * pEvent)
{
   if(ST_RECV == m_type)
   {
      return SRST_ERREND;
   }
}

TStepResult TTCPStep::recv(TMTPEvent * pEvent)
{
   if(ST_SEND == m_type)
   {
      return SRST_ERREND;
   }
   
}
