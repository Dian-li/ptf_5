/**
*
* @file      script.C
* @brief     define step class base function
* @author    wangqihua
* @History
*    1.0.0: wangqihua,  2017/08/06,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "step.h"
#include "mptevent.h"
#include "mptconst.h"

const char* g_sStepTypeStr[]=
{
    "send",
    "recv",
    "check"
};

TStep::TStep()
{
   m_protocol = P_UNKNOW;
   m_type = ST_SEND;
   m_timer = 0;
   m_next = NULL;
   m_child = NULL;
   m_parent = NULL;
}

TStep::~TStep()
{
   if(NULL != m_next)
   {
      delete m_next;
      m_next = NULL;
   }

   if(NULL != m_child)
   {
      delete m_child;
      m_child = NULL;
   }
   if(NULL != m_data.content)
   {
      delete []m_data.content;
      m_data.content = NULL;
      m_data.length = 0;
   }
}

TStep* TStep::next() const
{
   if(NULL != m_child)
   {
      return m_child;
   }
   else if(NULL != m_next)
   {
      return m_next;
   }
   else
   {
      return m_parent;
   }
}

TStep* TStep::parent() const
{
   return m_parent;
}

bool TStep::init(xmlNodePtr pNode)
{
   xmlChar* pValue = NULL;
   int      i = 0;
   int      length = 0;

   
   if(0 == xmlStrcmp(pNode->name, BAD_CAST("send")))
   {
      m_type = ST_SEND;
       printf("send-event");
   }
   else if(0 == xmlStrcmp(pNode->name, BAD_CAST("recv")))
   {
      m_type = ST_RECV;
   }
   else
   {
      printf("TStep init() find unknow step type(%s)", (char *)pNode->name);
      return false;
   }
   xmlNodePtr  pChildNode = pNode->xmlChildrenNode;
   while(NULL != pChildNode)
   {
       if(XML_CDATA_SECTION_NODE == pChildNode->type)
       {
           pValue = xmlNodeGetContent(pChildNode);
           if(unlikely(NULL == pValue))
           {
              printf("TStep init() get CDATA failed !");
              return false;
           }
           length = strlen((const char*)pValue);
           m_data.length = 0;
           m_data.content = new char[length+1];
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
           m_data.content[m_data.length] = 0;
           xmlFree(pValue);
           pValue = NULL;
           break;
       }
       pChildNode = pChildNode->next;
   }
   return true;
}

TStepResult TStep::run(TMPTEvent * pEvent)
{
   long double       step_begin = 0.0;
   long double       step_duration = 0.0;
   struct timeval    cur_t;
   struct timezone   cur_z;
   char              time_str[32] = {0};
   TStepResult              ret = SRST_ERREND;

   gettimeofday(&cur_t,&cur_z);
   snprintf(time_str, sizeof(time_str), "%d.%d", cur_t.tv_sec, cur_t.tv_usec);
   step_begin = strtold(time_str, NULL); 

   switch(m_type)
   {
      case ST_SEND:
      {
         ret = send(pEvent);
         break;
      }
      case ST_RECV:
      {
         ret = recv(pEvent);
         break;
      }
      default:
      {
         break;
      }
   }
      
   /*gettimeofday(&cur_t,&cur_z);
   snprintf(time_str, sizeof(time_str), "%d.%d", cur_t.tv_sec, cur_t.tv_usec);
   step_duration = strtold(time_str, NULL) - step_begin; 
   
   total_count++
   total_duration += step_duration;
   if(ret)
   {
      success_count++;
      success_duration += step_duration;
      if(step_duration > success_max)
      {
         success_max = step_duration;
      }
      if(step_duration < success_min)
      {
         success_min = step_duration;
      }
   }
   else
   {
      failure_count++
      failure_duration += step_duration;
      if(step_duration > failure_max)
      {
         failure_max = step_duration;
      }
      if(step_duration < failure_min)
      {
         failure_min = step_duration;
      }
   }*/
   return ret;
}
