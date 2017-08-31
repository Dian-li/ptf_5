/**
* @brief     define stdout step functions, just for test
* @author    chenggang
* @History
*    1.0.0: chenggang,  2018/08/30,  Initial version
*/

#include <stdio.h>
#include <pthread.h>

#include "stdoutstep.h"
#include "mptconst.h"

TStepResult TStdoutStep::send(TMPTEvent* pEvent)
{
    if(likely(NULL != m_data.content))
    {
        printf("%lu: %s\n", pthread_self(), m_data.content);
        fflush(stdout);
    }
    return SRST_SUCC;
}

TStepResult TStdoutStep::recv(TMPTEvent* pEvent)
{
    return SRST_ERREND;
}
