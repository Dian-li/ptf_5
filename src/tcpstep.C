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
#include "mptconst.h"

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
   bool ret = TStep::init(pNode);
   if(unlikely(!ret)) return ret;
   m_protocol = P_TCP;
   pValue = xmlGetProp(pNode, (const xmlChar *)"ip");
   if(unlikely(NULL == pValue))
   {
      printf("TTCPStep init() get destination ip address failed !");
      return false;
   }
   else
   {
      m_ip = (const char*)pValue;
   }
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"port");
   if(unlikely(NULL == pValue))
   {
      printf("TTCPStep init() get destination port failed !");
      return false;
   }
   else
   {
      m_port = atoi((const char*)pValue);
   }
   return true;
}


TStepResult TTCPStep::send(TMPTEvent * pEvent)
{
   if(ST_RECV == m_type)
   {
      return SRST_ERREND;
   }
}

TStepResult TTCPStep::recv(TMPTEvent * pEvent)
{
   if(ST_SEND == m_type)
   {
      return SRST_ERREND;
   }
   
}
