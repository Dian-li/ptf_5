/**
 *
 * @file      script.C
 * @brief     define script storage and interfaces
 * @author    chenggang
 * @History
 *    1.0.0: chenggang,  2017/06/19,  Initial version
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpointer.h>
#include <strings.h>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "script.h"
#include "step.h"
#include "mptconst.h"
#include "atomic.h"
#include "stdoutstep.h"
#include "tcpstep.h"
#include "httpstep.h"
//#include "dbstep.h"

using namespace std;
TScript::TScript(const char *file) {
	m_file = getenv("PTF_HOME");
	m_file += "/task/";
	m_file += file;
	//m_file += ".xml";
	m_script = NULL;
	m_redis = NULL;
	m_script_name = file;
	m_redis = new Redis();
	if (!m_redis->connect("127.0.0.1", 6379)) {
		cout << "redis connect error!" << endl;
	}

}

TScript::~TScript() {
	m_redis->free();
	delete m_redis;
	m_redis = NULL;
	exit();
}

bool TScript::init() {

	xmlDocPtr pDoc = NULL;
	xmlNodePtr pRoot = NULL;
	xmlNodePtr pNode = NULL;
	TStep *pStep = NULL;
	xmlKeepBlanksDefault(0);
	pDoc = xmlReadFile(m_file.c_str(), NULL, XML_PARSE_RECOVER);
	if (NULL == pDoc) {
		printf("ERROR: TScript can't open file[%s]!\n", m_file.c_str());
		return false;
	}
	pRoot = xmlDocGetRootElement(pDoc);
	if (NULL == pRoot) {
		printf("ERROR: TScript check XML file[%s] is invalid!\n",
				m_file.c_str());
		return false;
	}
	if (xmlStrcmp(pRoot->name, (const xmlChar *) "root")) {
		printf("TScript check XML type wrong, root node != root");
		xmlFreeDoc(pDoc);
		return false;
	}
	pNode = pRoot->xmlChildrenNode;
	TStep *pLast = NULL;
	while (pNode) //generate an step linklist
	{
		if (unlikely(XML_TEXT_NODE == pNode->type)) {
			pNode = pNode->next;
		}
		if (unlikely(NULL == pNode)) {
			break;
		}
		pStep = initProperty(pNode); //根据节点类型返回节点
		//assert(NULL != m_transaction);
		if( NULL != m_transaction){
			pStep->m_transaction = m_transaction; //every step hold a transaction instance
		}
		if (NULL == pStep) {
			break;
		}

		if (NULL == m_script) {
			m_script = pStep;
		}
		if (pLast) {
			pLast->m_next = pStep;
			pLast->m_child = pStep;
			pStep->m_parent = pLast;
		}
		pNode = pNode->next;
		pLast = pStep;
	}
	pLast->m_next = NULL;
	pLast->m_child = NULL;

	if (pNode)   //step init failed, delete all step.
	{
		if (NULL != m_script) {
			delete m_script;
		}
		xmlFreeDoc(pDoc);
		return false;
	}

	if (NULL == m_script) {
		printf("TScript init failed: can't get valid script");
		xmlFreeDoc(pDoc);
		return false;
	}

	xmlFreeDoc(pDoc);
	return true;

}

TStep *TScript::initProperty(xmlNodePtr pNode) {
	xmlChar *pValue = NULL;
	TStep *pStep = NULL;
	bool ret = false;
	pValue = xmlGetProp(pNode, (const xmlChar *) "protocol");

//    if (strcasecmp(PROTOCOL_HTTP, (const char *) pValue) == 0) {
//        pStep = new THTTPStep();
//    } else
//    if (strcasecmp(PROTOCOL_TCP, (const char *) pValue) == 0) {
//        pStep = new TTCPStep();
//    }
//    else if (strcasecmp(PROTOCOL_UDP, (const char *) pValue) == 0) {
//        pStep = new TUDPStep();
//    } else if (strcasecmp(PROTOCOL_FTP, (const char *) pValue) == 0) {
//        pStep = new TFTPStep();
//    } else if (strcasecmp(PROTOCOL_RMDB, (const char *) pValue) == 0) {
//        pStep = new TRMDBStep();
//    } else if (strcasecmp(PROTOCOL_STDOUT, (const char *) pValue) == 0) {
//        pStep = new TStdoutStep();
//    } else if (strcasecmp(PROTOCOL_RMDB, (const char *) pValue) == 0) {
//        printf("RMDB step type\n");
//        //pStep = new TRMDBStep();
//    }
	    if (strcasecmp(PROTOCOL_HTTP, (const char *) pValue) == 0) {
	        pStep = new THTTPStep();
	    }
	else if (strcasecmp(PROTOCOL_TCP, (const char *) pValue) == 0) {
		pStep = new TTCPStep();
		//add by dian
		xmlNodePtr pChildNode = pNode->xmlChildrenNode;
		const char *ip;
		int port;
		string address;
		if (m_connStr.size() <= 0) {
			while (NULL != pChildNode) {
				if (xmlNodeIsText(pChildNode)
						|| XML_CDATA_SECTION_NODE == pChildNode->type) {
					pChildNode = pChildNode->next;
					continue;
				}
				char * node_name = (char *) pChildNode->name;
				char * node_value = (char *) xmlNodeGetContent(pChildNode);
				if (string(node_name) == "ip") {
					if (NULL != node_value) {
						ip = node_value;
					} else {
						printf(
								"TTCPStep init() get destination ip address failed !\n");
						return NULL;
					}
				} else if (string(node_name) == "port") {
					if (NULL != node_value) {
						port = atoi(node_value);
					} else {
						printf(
								"TTCPStep init() get destination port failed !\n");
						return NULL;
					}
				}
				pChildNode = pChildNode->next;
			}
			std::stringstream ss;
			string s_ip = ip;
			ss << s_ip;
			ss << ":";
			ss << port;
			m_connStr = ss.str();
		}
	} else {
		printf("TScript initProperty() find unknow protocol type(%s)\n",
				pValue);
		return NULL;
	}
	ret = pStep->init(pNode);    //  初始化pStep的所有数据；

	if (false == ret) {
		printf("TScript sStep->init failed!\n");
		delete pStep;
		pStep = NULL;
	}
	return pStep;
}

TStep *TScript::enter() {
	uint64_t old;
	do {
		old = m_nUsing;
	} while (!CAS(&m_nUsing, old, old + 1));
	return m_script;
}

bool TScript::exit() {
	uint64_t old;
	do {
		old = m_nUsing;
	} while (!CAS(&m_nUsing, old, old - 1));
	do {
		old = m_nUsed;
	} while (!CAS(&m_nUsed, old, old + 1));
	return true;
}
//add by dian
void TScript::setTTransation(TTransation *tTransation) {
	m_transaction = tTransation;
}

string TScript::getConnStr() {
	return m_connStr;
}

void TScript::setCSocket(CSocket *cSocket) {
	m_cSocket = cSocket;
}

void TScript::m_send_st2redis() {
	TStep * pre_s = m_script;
	int total_count = 0;
	int success_count = 0;
	int failure_count = 0;
	double total_duration = 0;
	double success_duration = 0; //s.us
	double success_max = 0;
	double success_min = 0;
	double failure_duration = 0; //s.us
	double failure_max = 0;
	double failure_min = 0;

	while (pre_s) {
		total_count += pre_s->m_st.total_count;
		success_count += pre_s->m_st.success_count;
		failure_count += pre_s->m_st.failure_count;
		total_duration += pre_s->m_st.total_duration;
		success_duration += pre_s->m_st.success_count;
		failure_duration += pre_s->m_st.failure_count;
		success_max += pre_s->m_st.success_max;
		success_min += pre_s->m_st.success_min;
		failure_max += pre_s->m_st.failure_max;
		failure_min += pre_s->m_st.failure_min;
		pre_s = pre_s->next();
	}

	double avg_duration = total_duration / ((double) total_count);
	char time_str[60];
	snprintf(time_str, sizeof(time_str), "%f", avg_duration);

	m_redis->m_redis_mutex.lock();
	m_redis->set("avg_delay", string(time_str));
	m_redis->set("total_count", to_string(total_count));
	m_redis->set("success_count", to_string(success_count));
	m_redis->set("failure_count", to_string(failure_count));
	m_redis->set("total_duration", to_string(total_duration));

	//m_redis->set("success_duration", to_string(success_duration));
	// m_redis->set("failure_duration", to_string(failure_duration));
	m_redis->set("success_max", to_string(success_max));
	m_redis->set("success_min", to_string(success_min));
	//m_redis->set("failure_max", to_string(failure_max));
	//m_redis->set("failure_min", to_string(failure_min));
	m_redis->m_redis_mutex.unlock();
	cout << "---total_num---" << total_count << "---total_duration---"
			<< total_duration << "---total_failure--" << failure_count << endl;
}

