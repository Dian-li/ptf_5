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

#include "script.h"
#include "step.h"
#include "mptconst.h"
#include "atomic.h"
#include "stdoutstep.h"
#include "tcpstep.h"
//#include "dbstep.h"

TScript::TScript(const char *file) {
    m_file = getenv("PTF_HOME");
    m_file += "/task/";
    m_file += file;
    m_file += ".xml";
    m_script = NULL;
}

TScript::~TScript() {
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
        printf("ERROR: TScript check XML file[%s] is invalid!\n", m_file.c_str());
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
        pStep = initProperty(pNode);//根据节点类型返回节点
        if (NULL == pStep) {
            break;
        }

        if (NULL == m_script) {
            m_script = pStep;
        }
        if (pLast) {
            pLast->m_next = pStep;
        }
        pNode = pNode->next;
        pLast = pStep;
    }

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
    if (strcasecmp(PROTOCOL_TCP, (const char *) pValue) == 0) {
        pStep = new TTCPStep();
    }
    else {
        printf("TScript initProperty() find unknow protocol type(%s)\n", pValue);
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


