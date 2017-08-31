/**
*
* @file      manager.C
* @brief     define manager module, main thread call it
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <event2/buffer.h>
#include <stdlib.h>
#include <string.h>

#include "manager.h"
#include "ptfconfig.h"
#include "mptconst.h"

static void httpd_cb(struct evhttp_request *req, void *arg);
static void httpd_ptf_cb(struct evhttp_request *req, void *arg);

TManager::TManager()
{
    m_base = event_base_new();
    m_httpd = NULL;
}

TManager::~TManager()
{
    if(likely(NULL != m_httpd))
    {
        evhttp_free(m_httpd);
        m_httpd = NULL;
    }
    if(likely(NULL != m_base))
    {
        event_base_free(m_base);
        m_base = NULL;
    }
}

int TManager::init()
{
    int ret = startHttpServer();
    if(unlikely(0 != ret)) return ret;
    ret = m_threadPool.init(TPTFConfig::getInstance()->getWorkers());
    if(unlikely(0 != ret)) return ret;
}

int TManager::startHttpServer()
{
    if(unlikely(NULL != m_httpd))
    {
        evhttp_free(m_httpd);
    }
    m_httpd = evhttp_new(m_base);
    if(unlikely( 0 != evhttp_bind_socket(m_httpd , TPTFConfig::getInstance()->getListenAddr(),
                                TPTFConfig::getInstance()->getManagerPort())))
    {
        printf("open manager interface httpd failed.\n");
        return -1;
    }
    evhttp_set_timeout(m_httpd, 120);
    evhttp_set_gencb(m_httpd, httpd_cb, this);
    //evhttp_set_cb(m_httpd, "/ptf", httpd_ptf_cb, this);
    
    return 0;
}

void TManager::loop()
{
    event_base_dispatch(m_base);
}

void TManager::processHttpReq(struct evhttp_request *req)
{
    evhttp_cmd_type type = evhttp_request_get_command(req);
    const char * str = evhttp_request_get_host(req);
    const char *decode_uri =  evhttp_request_get_uri(req) ;
    char *url = evhttp_decode_uri(decode_uri);
    evbuffer *buff = evbuffer_new();
    printf( "[%s][%s]-->[%s]\n" , str , decode_uri, url);
    if(0==strcmp("/", url))
    {
        evbuffer_add(buff, "It works!", 9);
        evhttp_send_reply( req, HTTP_OK, "OK", buff);
    }
    else if(0 == strncmp("/ptf", url, 4))
    {
        char *p = NULL, *q = NULL;
        int nLen = strlen(url);
        p = url + 4 + 1;
        if(p < url + nLen)
        {
            q = strchr(p, '/');
            if(NULL != q)
            {
                *q = 0;
            }
            TScript* pScript = new TScript(p);
            if(NULL != q) *q = '/';
            bool ret = false;
            if(likely(NULL != pScript))
            {
                ret = pScript->init();
            }
            if(likely(ret))
            {
                vector < int > workers;
                int nId = chooseWorkers(workers);
                if(unlikely(nId < 0))
                {
                    evhttp_send_reply( req, HTTP_INTERNAL, "Internal Error", NULL);
                }
                else 
                {                    
                    if(0 == nId)
                    {
                        for(int i=0; i<m_threadPool.size(); ++i)
                        {
                            TCmdStartEvent *pCmd = new TCmdStartEvent(pScript, 2, 25, 0); 
                            m_threadPool.order(pCmd, i);
                        }
                    }
                    else
                    {
                        for(vector<int>::iterator iter = workers.begin(); iter != workers.end(); ++iter)
                        {
                            TCmdStartEvent *pCmd = new TCmdStartEvent(pScript, 2, 25, 0);
                            m_threadPool.order(pCmd, *iter);
                        }
                    }
                    evbuffer_add(buff, "start task successfully!", 24);
                    evhttp_send_reply( req, HTTP_OK, "OK", buff);
                }
            }
            else
            {
                evbuffer_add(buff, "load task failed!", 17);
                evhttp_send_reply( req, HTTP_INTERNAL, "Internal Error", NULL);
            }
        }
        else
        {
            evhttp_send_reply( req, HTTP_NOTFOUND, "Not Found", NULL);
        }
    }
    else
    {
        evhttp_send_reply( req, HTTP_NOTFOUND, "Not Found", NULL);
    }

    evbuffer_free(buff);
    if(likely( NULL != url  ))
    {
       free(url);
    }
    return;
}

int TManager::chooseWorkers(vector < int > & workers)
{
    //TODO
    return 0;
}

void httpd_cb(struct evhttp_request *req, void *arg)
{
    ((TManager*)arg)->processHttpReq(req);
}

void httpd_ptf_cb(struct evhttp_request *req, void *arg)
{
    const char * str = evhttp_request_get_host(req);
    const char *decode_uri =  evhttp_request_get_uri(req) ;
    evbuffer *buff = evbuffer_new();
    evbuffer_add(buff, "Welcome to mpt!", 15);
    printf( "[%s][%s]\n" , str , decode_uri);
    evhttp_send_reply( req, HTTP_OK, "OK", buff);
    evbuffer_free(buff);
}
