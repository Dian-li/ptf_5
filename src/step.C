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
#include <iostream>

#include "step.h"
#include "mptevent.h"
#include "mptconst.h"
using namespace std;
const char *g_sStepTypeStr[] = {"send", "recv", "check"};

TStep::TStep() {
  m_protocol = P_UNKNOW;
  m_type = ST_SEND;
  m_timer = 0;
  m_next = NULL;
  m_child = NULL;
  m_parent = NULL;
}

TStep::~TStep() {
  if (NULL != m_next) {
    delete m_next;
    m_next = NULL;
  }

  if (NULL != m_child) {
    delete m_child;
    m_child = NULL;
  }
  if (NULL != m_data.content) {
    delete[] m_data.content;
    m_data.content = NULL;
    m_data.length = 0;
  }
}

TStep *TStep::next() const {
  if (NULL != m_child) {
    return m_child;
  } else if (NULL != m_next) {
    return m_next;
  } else {
    //return m_parent;
    return NULL;
  }
}

TStep *TStep::parent() const {
  return m_parent;
}

bool TStep::init(xmlNodePtr pNode) {
  xmlChar *pValue = NULL;
  int i = 0;
  int length = 0;

  if (0 == xmlStrcmp(pNode->name, BAD_CAST("send"))) {
    m_type = ST_SEND;
    //printf("send-event");
  } else if (0 == xmlStrcmp(pNode->name, BAD_CAST("recv"))) {
    m_type = ST_RECV;
  } else {
    printf("TStep init() find unknow step type(%s)", (char *) pNode->name);
    return false;
  }
  xmlNodePtr pChildNode = pNode->xmlChildrenNode;
  while (NULL != pChildNode) {
    if (XML_CDATA_SECTION_NODE == pChildNode->type) {
      pValue = xmlNodeGetContent(pChildNode);
      if (unlikely(NULL == pValue)) {
        printf("TStep init() get CDATA failed !");
        return false;
      }
      length = strlen((const char *) pValue);
      m_data.length = 0;
      m_data.content = new char[length + 1];
      for (i = 0; i < length; i++) {
        if ('\\' == pValue[i]
            && ('x' == pValue[i + 1] || 'X' == pValue[i + 1])) {
          m_data.content[m_data.length++] = (unsigned char) pValue[i
              + 2] * 16 + (unsigned char) pValue[i + 3];
          i += 3;
        } else {
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

TStepResult TStep::run(TMPTEvent *pEvent) {
  long double step_begin = 0.0;
  long double step_duration = 0.0;
  struct timeval start_time;
  struct timeval end_time;
  struct timeval diff_time;
  struct timeval cur_t;
  struct timezone cur_z;
  char time_str[32] = {0};
  TStepResult ret = SRST_ERREND;
  gettimeofday(&start_time, NULL);
//	gettimeofday(&cur_t, &cur_z);
//	snprintf(time_str, sizeof(time_str), "%d.%d", cur_t.tv_sec, cur_t.tv_usec);
//	step_begin = strtold(time_str, NULL);

  switch (m_type) {
    case ST_SEND: {
      supsend();
      ret = send(pEvent);
      printf("send finished,ret = %d\n", ret);
      break;
    }
    case ST_RECV: {
      suprecv();
      ret = recv(pEvent);
      printf("recv finished,ret = %d\n", ret);
      break;
    }
    default: {
      printf("others,ret = %d\n", ret);
      break;
    }
  }
  //===statistics===
  gettimeofday(&end_time, NULL);
  timeval_subtract(&diff_time, &start_time, &end_time);
  //cout<<"diff_time is "<<diff_time.tv_sec<<"   "<<diff_time.tv_usec<<endl;
  step_duration = (double) diff_time.tv_sec
      + ((double) diff_time.tv_usec) / 1000000.0;

  m_st.total_count++;
  //m_st.duration_vector.push_back(step_duration);
  m_st.total_duration += step_duration;
  if (ret == SRST_SUCC) {
    m_st.success_count++;
    //printf("m_st.success_count is %d\n",m_st.success_count);
    m_st.success_duration += step_duration;
    if (step_duration > m_st.success_max) {
      m_st.success_max = step_duration;
    }
    if (step_duration < m_st.success_min) {
      m_st.success_min = step_duration;
    }
  } else if (ret == SRST_ERREND) {
    m_st.failure_count++;
    m_st.failure_duration += step_duration;
    if (step_duration > m_st.failure_max) {
      m_st.failure_max = step_duration;
    }
    if (step_duration < m_st.failure_min) {
      m_st.failure_min = step_duration;
    }
  }
  printf("static finished,ret=%d!\n", ret);
  return ret;
}

void TStep::setTTransaction(TTransation *tTransation) {
  m_transaction = tTransation;
}
