/**
*
* @file      step.h
* @brief     define script step class, script run with it
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*    1.0.1: wangqihua,  2017/08/10,  add TStepStatistics
*/

#ifndef _STEP_H
#define _STEP_H

#include <libxml/parser.h>
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
   P_UNKNOW = 0,
   P_HTTP = 1,
   P_TCP = 2,
   P_UDP = 3,
   P_FTP = 4,
   P_RMDB = 5
};

extern const char* g_sStepTypeStr[];

struct TCode
{
   int   length;
   char* content;
   
   TCode()
   {
      length = 0;
      content = NULL;
   }
};

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

class TMPTEvent;
class TStep
{
friend class TScript;
public:
   TStep();
   virtual ~TStep();
   TStep* next() const;
   TStep* parent() const;
   virtual TStepResult send(TMPTEvent* pEvent){return SRST_ERREND;}
   virtual TStepResult recv(TMPTEvent* pEvent){return SRST_ERREND;}
   virtual bool init(xmlNodePtr pNode);
   TStepResult  run(TMPTEvent* pEvent);
protected:
   TProtocolType     m_protocol;
   TStepStatistics   m_st;
   TStepType         m_type;
   int               m_timer;
   TStep*            m_next;
   TStep*            m_child;
   TStep*            m_parent;
   TCode    m_data;
};

#endif

