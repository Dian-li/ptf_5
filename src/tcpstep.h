/**
*
* @file      tcpstep.h
* @brief     define tcp step class, script run with it
* @author    wangqihua
* @History
*    1.0.0: wangqihua,  2017/08/07,  Initial version
*/

#ifndef _TCPSTEP_H
#define _TCPSTEP_H

#include "step.h"

class TTCPStep : public TStep
{
public:
   bool init(xmlNodePtr pNode);
   bool send(int ti);
   bool recv(int ti);
private:
   int      port;
   string   ip;
   TCode    data;
}

#endif
