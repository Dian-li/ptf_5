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
#include "socketpool.h"

class TTCPStep : public TStep {
public:
    TTCPStep();

    ~TTCPStep();

    bool init(xmlNodePtr pNode);

    TStepResult send(TMPTEvent *pEvent);

    TStepResult recv(TMPTEvent *pEvent);

private:
    int    m_port;
    string m_ip;
    string m_connStr;
    CSocket* m_cSocket;
    CSocketPool * m_conn_socket;
};

#endif
