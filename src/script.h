/**
*
* @file      script.h
* @brief     define script storage and interfaces, it must be thread safe
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _SCRIPT_H
#define _SCRIPT_H

#include "redis.h"
#include <string>
using std::string;
#include <stdint.h>
#include <libxml/parser.h>

#include "socketpool.h"
#include "transaction.h"

class TStep;
class TScript
{
public:
    TScript(const char* file);
    ~TScript();
    bool init();
    TStep* enter();
    bool exit();
    void setTTransation(TTransation *tTransation);//add by dian
    string getConnStr();
    void setCSocket(CSocket *cSocket);
    void m_send_st2redis();
private:
    TStep* initProperty(xmlNodePtr pNode);

public:
    string m_protocol;

private:
    string m_file;
    string m_lastModify;
    TStep* m_script;
    string m_script_name;
    volatile bool m_bTobeRemoved;
    volatile uint64_t m_nUsing;
    volatile uint64_t m_nUsed;
    TTransation *m_transaction;//add by dian
    string m_connStr;
    CSocket *m_cSocket;
    Redis * m_redis;


};

#endif
