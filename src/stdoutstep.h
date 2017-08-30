/**
* @brief     define stdout step class, just for test
* @author    chenggang
* @History
*    1.0.0: chenggang,  2018/08/30,  Initial version
*/

#ifndef _STDOUTSTEP_H
#define _STDOUTSTEP_H

#include "step.h"

class TStdoutStep : public TStep
{
public:
   TStdoutStep(){}
   ~TStdoutStep(){}
   TStepResult send(TMPTEvent* pEvent);
   TStepResult recv(TMPTEvent* pEvent);
};

#endif

