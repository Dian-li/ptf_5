/**
*
* @file      st.h
* @brief     define statistics class, include system load and step implement
* @author    wangqihua
* @History
*    1.0.0: wangqihua,  2017/08/07,  Initial version
*/

#ifndef  _ST_H
#define  _ST_H

#include <string>
using std::string;

struct TCPUST
{
   string         process_name[128];
   long double    cpu_total;
   long long      cpu_count;
   double         cpu_load;
   double         cpu_max;
   double         cpu_min;
};

struct TSystemST
{
   
   
};




#endif