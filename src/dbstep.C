/**
  *
  * @file      tcpstep.C
  * @brief     define tcp step function
  * @author    wangqihua
  * @History
  *    1.0.0: wangqihua,  2017/08/07,  Initial version
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<soci/soci.h>
#include<soci/postgresql/soci-postgresql.h>
//need to add '-lpq -lsoci_core -lsoci_postgresql -ldl'
#include <iostream>
#include "dbstep.h"

using std::cout;
using std::endl;
using namespace std;
using namespace soci;

TRMDBStep::TRMDBStep(){

}

TRMDBStep::~TRMDBStep(){

}

bool TRMDBStep::init(xmlNodePtr pNode){
	if  (xmlNodeIsText(pNode)) {
		pNode = pNode->next;
	}// filter the text node;
	if (!xmlStrcmp(pNode->name, BAD_CAST("send"))) {//!
		m_type = ST_SEND;
	} else if (!xmlStrcmp(pNode->name, BAD_CAST("recv"))) {
		m_type = ST_RECV;
	} else {
		printf("TDBStep init() find unknow step type(%s)", (char *)pNode->name);
		return false;
	}

	m_protocol = P_RMDB;

	xmlNodePtr p_child = pNode->xmlChildrenNode;
	while (p_child) {
		if (xmlNodeIsText(p_child)) {
			p_child = p_child->next;
			continue;
		}
		char * node_name = (char *)p_child->name;
		char * node_value = (char *)xmlNodeGetContent(p_child);

		if (string(node_name) == "wait") {
			m_conn.m_wait = string(node_value);
		} else if (string(node_name) == "host") {
			m_conn.m_host = string(node_value);
		} else if (string(node_name) == "port") {
			m_conn.m_port = string(node_value);
		} else if (string(node_name) == "username") {
			m_conn.m_username = string(node_value);
		} else if (string(node_name) == "password") {
			m_conn.m_password = string(node_value);
		} else if (string(node_name) == "db_name") {
			m_conn.m_dbname = string(node_value);
		} else if (string(node_name) == "sql") {
			m_conn.m_sql = string(node_value);
		} else if (string(node_name) == "protocol"){
			m_conn.m_connType = string(node_value);
		} else if (string(node_name) == "result") {
            m_result = string(node_value);
		} else {
			printf("invalid node :%s\n", node_name);
			return false;
		}
		cout<<"  get node name:"<<node_name<<"; and content="<<    node_value<<endl;
		p_child = p_child->next;
	}
	m_connStr = m_conn.m_connType + "://" + "host="+m_conn.m_host + " dbname="+m_conn.m_dbname + " user="+m_conn.m_username + " password="+m_conn.m_password+" port="+m_conn.m_port;
	m_sqlStr = m_conn.m_sql;
	cout<<"m_connStr = "<<m_connStr<<"### m_sqlStr="<<m_sqlStr<<endl;
	return true;
}

TStepResult TRMDBStep::send(TMPTEvent * pEvent) {
	if(ST_RECV == m_type)
	{
		return SRST_ERREND;
	}
	session sql;
	try {
		sql.open(m_connStr);
		rowset<row> rs = (sql.prepare<<m_sqlStr);
		for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); it++) {
			row const & r = *it;
			cout<<"stu_no:"<<r.get<int>(0)<<endl;
		}
		return SRST_SUCC;
	} catch (exception const & e) {
		cerr<<"error::"<<e.what()<<endl;
		return SRST_ERREND;
	}
	sql.close();
}
TStepResult TRMDBStep::recv(TMPTEvent * pEvent)
{
	if(ST_SEND == m_type)
	{
		return SRST_ERREND;
	}
}




