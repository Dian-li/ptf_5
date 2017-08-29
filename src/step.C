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
#include "step.h"
#include "mptevent.h"

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

TStepResult TStep::run(TMTPEvent * pEvent)
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
