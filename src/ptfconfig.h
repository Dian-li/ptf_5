/**
*
* @file      ptfconfig.h
* @brief     read ptf config
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _PTFCONFIG_H
#define _PTFCONFIG_H

#include <libconfig.h>

class TPTFConfig
{
public:
    static TPTFConfig* getInstance()
    {
        if(NULL == m_instance)
        {
            m_instance = new TPTFConfig();
        }
        return m_instance;
    }
    ~TPTFConfig();
    bool init(const char* file);
    void print();
    unsigned short getManagerPort() const {return (unsigned int)(m_managePort & 0xffff);}
    const char* getListenAddr() const {return m_listenAddr;}
    int getWorkers() const {return m_nWorker;}
protected:
    TPTFConfig();
private:
    static TPTFConfig* m_instance;
    config_t* m_conf;
    long m_managePort;
    const char* m_listenAddr;
    long m_nWorker;
};

#endif
