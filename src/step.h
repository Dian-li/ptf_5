/**
*
* @file      step.h
* @brief     define script step class, script run with it
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _STEP_H
#define _STEP_H

#include "st.h"

enum TStepType
{
    ST_SEND,
    ST_RECV,
    ST_CHECK
};

enum TStepResult
{
    SRST_ERREND = -1,
    SRST_SUCC = 0,
    SRST_SUBRETURN,
    SRST_NEXTEVENT
};

enum TProtocolType
{
   P_UNKNOW = 0;
   P_HTTP = 1;
   P_TCP = 2;
   P_UDP = 3;
   P_FTP = 4;
   P_RMDB = 5
};

const char* g_sStepTypeStr[]=
{
    "send",
    "recv",
    "check"
};

struct TCode
{
   int   length;
   char* content;
   
   TCode()
   {
      length = 0;
      content = NULL;
   }
}

struct TStepStatistics
{
   long long   total_count;
   long long   success_count;
   long long   failure_count;
   long double total_duration;   //s.us
   long double success_duration; //s.us
   long double success_max;
   long double success_min;
   long double failure_duration; //s.us
   long double failure_max;
   long double failure_min;
   
   TStepStatistics()
   {
      total_count = 0;
      success_count = 0;
      failure_count = 0;
      total_duration = 0.0;
      success_duration = 0.0;
      success_max = 0.0;
      success_min = 0.0;
      failure_duration = 0.0;
      failure_max = 0.0;
      failure_min = 0.0;
   }
};

#define STEP_TYPE_STR(x) (x>=0 && x<sizeof(g_sStepTypeStr)/sizeof(g_sStepTypeStr[0]) ? g_sStepTypeStr[x] : "unknow")

class TStep
{
public:
   bool run(int ti);
   virtual bool   send(int ti);
   virtual bool   recv(int ti);
   //virtual void*  send(int ti);
   //virtual void*  recv(int ti);
   virtual bool   init(xmlNodePtr pNode);
   //virtual TStepResult  run(TMTPEvent* pEvent)=0;
private:
   TProtocolType     m_protocol;
   TStepStatistics   m_st;
   TStepType         m_type;
   int               m_timer;
   TStep*            m_next;
   TStep*            m_child;
   TStep*            m_parent;
};

#endif

