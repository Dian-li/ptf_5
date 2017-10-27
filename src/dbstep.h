/*@file      tcpstep.h
* @brief     define tcp step class, script run with it
* @author    wangqihua
* @History
*    1.0.0: wangqihua,  2017/08/07,  Initial version
	1.0.1: liuxiaochuang, 2017/08/30,
*/

#ifndef _DBSTEP_H
#define _DBSTEP_H

#include "step.h"
#include "mptevent.h"

struct TDBConn {
	string  m_wait;
	string	m_connType;
	string	m_port;
	string  m_host;
	string  m_username;
	string  m_password;
	string  m_dbname;
	string  m_sql;
};

class TRMDBStep : public TStep{
public:
	TRMDBStep();
	~TRMDBStep();
	bool init(xmlNodePtr pNode);
	TStepResult send(TMPTEvent * pEvent);
	TStepResult recv(TMPTEvent * pEvent);
private:
	TDBConn			m_conn;
	string          m_result;
	TProtocolType   m_protocol;
	string			m_connStr;
	string			m_sqlStr;
};
#endif
