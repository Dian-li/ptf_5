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
#include "tcpstep.h"

TTCPStep::TTCPStep()
{
   port = 0;
}

TTCPStep::~TTCPStep()
{
   port = 0;
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
      ip = pValue;
   }
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"port");
   if(NULL == pValue)
   {
      printf("TTCPStep init() get destination port failed !");
      return false;
   }
   else
   {
      port = atoi(pValue);
   }
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"CDATA");
   if(NULL == pValue)
   {
      printf("TTCPStep init() get CDATA failed !");
      return false;
   }
   length = strlen(pValue);
   data.length = 0;
   data.content = new char[length];
   for(i = 0; i < length; i++)
   {
      if('\\' == pValue[i] && ('x' == pValue[i + 1] || 'X' == pValue[i + 1]))
      {
         data.content[data.length++] = (unsigned char)pValue[i + 2] * 16 + (unsigned char)pValue[i + 3];
         i += 3;
      }
      else
      {
         data.content[data.length++] = pValue[i];
      }
   }
}


bool TTCPStep::send(int ti)
{
   if(ST_RECV == m_type)
   {
      return false;
   }
}

bool TTCPStep::recv(int ti)
{
   if(ST_SEND == m_type)
   {
      return false;
   }
   
}
