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
#include <sys/time.h>

#include<soci/soci.h>
#include<soci/postgresql/soci-postgresql.h>
#include<soci/mysql/soci-mysql.h>
//need to add '-lpq -lsoci_core -lsoci_postgresql -ldl'
#include <iostream>


#include "dbstep.h"
#include "cmdevent.h"
#include "connpool.h"

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
		} else if (string(node_name) == "db_type"){
			m_conn.m_connType = string(node_value);
		} else if (string(node_name) == "result") {
            m_result = string(node_value);
		} else {
			printf("invalid node :%s\n", node_name);
			return false;
		}
		cout<<"  get node name:"<<node_name<<"; and content="<<    node_value<<endl;
		p_child = p_child->next;
		if (xmlNodeIsText(p_child)) {
			p_child = p_child->next;
			//continue;
		}
	}
	m_connStr = m_conn.m_connType + "://" + "host="+m_conn.m_host + " dbname="+m_conn.m_dbname + " user="+m_conn.m_username + " password="+m_conn.m_password+" port="+m_conn.m_port;
	m_sqlStr = m_conn.m_sql;
	m_sqlType = m_conn.m_sql_type;
	cout<<"m_connStr = "<<m_connStr<<"### m_sqlStr="<<m_sqlStr<<"; and sql_type="<<m_conn.m_sql_type<<endl;
	return true;
}

TStepResult TRMDBStep::send(TMPTEvent * pEvent) {
	if(ST_RECV == m_type)
	{
		return SRST_ERREND;
	}
	if (pEvent == NULL) {
        //cout<<"in TRMDBStep::send: pEvent is NULL"<<endl;
	}
    //unordered_map<string, TDBConnPool*>* conn_map_ptr = TDBConnMap::Instance();
    db_map_mutex.lock();
	if (conn_map.find(m_connStr) == conn_map.end()) {
        printf("###### there is no such conn_str in map create it:%s######\n", m_connStr.c_str());
        conn_map[m_connStr] = new TDBConnPool(CONNPOOLSIZE, m_connStr);
        printf("\nthe size of map is :%d\n\n", conn_map.size());
	}
	db_map_mutex.unlock();

	TDBConnPool * db_conn_pool_ptr = conn_map[m_connStr];

	struct timeval start, stop ,diff;
	gettimeofday(&start, 0);

	size_t pool_index = db_conn_pool_ptr->lease();
	session &sql = db_conn_pool_ptr->at(pool_index);
	using namespace soci;
	try {
        sql << m_sqlStr;
	} catch (soci::mysql_soci_error const & e) {
		cerr<< "MySQL error:"<<e.err_num_<<" "<<e.what()<<endl;
		return SRST_ERREND;
	} catch (std::exception const & e) {
		cerr<<"error, in TRMDBStep::send::"<<e.what()<<endl;
		return SRST_ERREND;
	}
	db_conn_pool_ptr->give_back(pool_index);

	gettimeofday(&stop, 0);
	if (timeval_subtract(&diff, &start, &stop) == 0) {
        //cout<<"it takes "<<diff.tv_sec<<" seconds and "<<diff.tv_usec<<" microseconds"<<endl;
	}
	return SRST_SUCC;
}
TStepResult TRMDBStep::recv(TMPTEvent * pEvent)
{
	if(ST_SEND == m_type)
	{
		return SRST_ERREND;
	}
}

/*TStepResult TRMDBStep::run(TCmdStartEvent * pEvent)
{
   long double       step_begin = 0.0;
   long double       step_duration = 0.0;
   struct timeval    cur_t;
   struct timezone   cur_z;
   char              time_str[32] = {0};
   TStepResult              ret = SRST_ERREND;

   gettimeofday(&cur_t,&cur_z);
   snprintf(time_str, sizeof(time_str), "%d.%d", cur_t.tv_sec, cur_t.tv_usec);
   step_begin = strtold(time_str, NULL);

   switch(m_type)
   {
      case ST_SEND:
      {
         ret = send(pEvent);
         break;
      }
      case ST_RECV:
      {
         ret = recv(pEvent);
         break;
      }
      default:
      {
         break;
      }
   }


   return ret;
}*/




