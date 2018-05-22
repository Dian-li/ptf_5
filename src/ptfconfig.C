/**
*
* @file      ptfconfig.C
* @brief     read ptf config
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptfconfig.h"
#include "mptconst.h"

TPTFConfig* TPTFConfig::m_instance = NULL;

TPTFConfig::TPTFConfig()
{
    m_conf = NULL;
    m_managePort = 13388;
    m_listenAddr = "0.0.0.0";
}

TPTFConfig::~TPTFConfig()
{
    if(likely(NULL != m_conf))
    {
        config_destroy(m_conf);
        m_conf = NULL;
    }
}

bool TPTFConfig::init(const char* file)
{
    if(NULL == file || 0 == *file) return false;
    const char* pFile = file;
    char filePath[512];
    if(likely('/' != *file))
    {
        sprintf(filePath, "%s/etc/%s", getenv("PTF_HOME"), file);
        pFile = filePath;
    }
    config_t* conf = new config_t;
    config_init(conf);
    int ret = config_read_file(conf, pFile);
    //config_lookup_int(conf, "workers", &m_nWorker);
    //printf("m_nWorker:%d\n", m_nWorker);
    if(unlikely(0 == ret))
    {
        printf("load config %s failed, configuration not changed.\n", pFile);
        return false;
    }
    if(NULL != m_conf)
    {
        config_destroy(m_conf);
    }
    m_conf = conf;
    config_lookup_int(conf, "manager_port", &m_managePort);
    config_lookup_string(conf, "listen", &m_listenAddr);
    config_lookup_int(conf, "workers", &m_nWorker);
    if(unlikely(m_nWorker > 65535 || m_nWorker < 1))
    {
        m_nWorker = 4;
    }
    printf("m_nWorker:%d\n", m_nWorker);
    return true;
}

void TPTFConfig::print()
{
    printf("manager_port: %d\n", m_managePort);
    printf("      listen: %s\n", m_listenAddr);
    printf("     workers: %d\n", m_nWorker);
    return;
}
