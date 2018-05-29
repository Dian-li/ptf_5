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
#include <iostream>
#include <map>
#include<fstream>
#include <cstring>

#include "manager.h"
#include "ptfconfig.h"
#include "mptconst.h"
#include "json/json.h"
#include "model/testcase.h"

using namespace std;

static void httpd_cb(struct evhttp_request *req, void *arg);
static void httpd_ptf_cb(struct evhttp_request *req, void *arg);

TManager::TManager() {
    m_base = event_base_new();
    m_httpd = NULL;
}

TManager::~TManager() {
    if (likely(NULL != m_httpd)) {
        evhttp_free(m_httpd);
        m_httpd = NULL;
    }
    if (likely(NULL != m_base)) {
        event_base_free(m_base);
        m_base = NULL;
    }
}

int TManager::init() {
    int ret = startHttpServer();
    if (unlikely(0 != ret))
        return ret;
    printf("conf works:%d\n", TPTFConfig::getInstance()->getWorkers());

    ret = m_threadPool.init(TPTFConfig::getInstance()->getWorkers());
    if (unlikely(0 != ret))
        return ret;
}

int TManager::startHttpServer() {
    if (unlikely(NULL != m_httpd)) {
        evhttp_free(m_httpd);
    }
    m_httpd = evhttp_new(m_base);
    if (unlikely(
            0 != evhttp_bind_socket(m_httpd, TPTFConfig::getInstance()->getListenAddr(), TPTFConfig::getInstance()->getManagerPort()))) {
        printf("open manager interface httpd failed.\n");
        return -1;
    }
    evhttp_set_timeout(m_httpd, 120);
    evhttp_set_gencb(m_httpd, httpd_cb, this);
    //evhttp_set_cb(m_httpd, "/ptf", httpd_ptf_cb, this);

    return 0;
}

void TManager::loop() {
    event_base_dispatch(m_base);
}

void TManager::processHttpReq(struct evhttp_request *req) {
    evhttp_cmd_type type = evhttp_request_get_command(req);
    const char * str = evhttp_request_get_host(req);
    const char *decode_uri = evhttp_request_get_uri(req);
    char *url = evhttp_decode_uri(decode_uri);
    evbuffer *buff = evbuffer_new();
    map<string, string> kvmap;
    printf("[%s][%s]-->[%s]\n", str, decode_uri, url);
    if (0 == strcmp("/", url)) {
        evbuffer_add(buff, "It works!", 9);
        evhttp_send_reply(req, HTTP_OK, "OK", buff);
    } else if (0 == strncmp("/ptf", url, 4)) {
        evbuffer_add(buff, "start task successfully!", 24);
        evhttp_send_reply(req, HTTP_OK, "OK", buff);
        if (0 == strncmp("/ptf/new_test", url, 18)) {  //传输参数
            printf("p(url) = %s\n", url);
            evbuffer* req_body_buffer = evhttp_request_get_input_buffer(req);
            int req_body_len = evbuffer_get_length(req_body_buffer);
            char req_body[req_body_len];
            evbuffer_remove(req_body_buffer, req_body, req_body_len);
            if (req_body[0] != '{') {
                evbuffer_add(buff, "BAD REQUEST!", 24);
                evhttp_send_reply(req, HTTP_BADREQUEST, "ERROR", buff);
            } else {
                Json::Reader reader;
                Json::Value value;
                Testcase* testcase = new Testcase();
                if (reader.parse(string(req_body), value)) {
                    testcase->tps = value["tps"].asInt();;
                    testcase->duration = value["duration"].asInt();
                    testcase->concurrenceMode = value["concurrenceMode"].asInt();
                    testcase->timedelay =  value["timedelay"].asInt();
                    testcase->threshold = value["threshold"].asInt();
                    testcase->pressureMode = value["pressureMode"].asInt();
                    testcase->protocol = value["protocol"].asString();
                    testcase->filename = value["scriptName"].asString();
                    testcase->file = value["file"].asString();
                    testcase->step = value["step"].asInt();
                    testcase->targetTps = value["targetTps"].asInt();
                }
                for (auto &item : kvmap) {
                    std::cout << "key:" << item.first << ";value:" << item.second << std::endl;
                }
                string ptf_dir = getenv("PTF_HOME");
                ptf_dir += "/task/";
                string filename = testcase->filename;
                string protocol = testcase->protocol;

                ptf_dir += filename;
                ofstream fs;
                fs.open(ptf_dir.c_str(), ios::out);
                if (!fs) {
                    evbuffer_add(buff, "Can not create the file!", 23);
                    evhttp_send_reply(req, HTTP_BADREQUEST, "Request Error",
                    NULL);
                }
                fs << testcase->file;
                fs.close();
                TScript* pScript = new TScript(filename.c_str());
                bool ret = false;
                if (likely(NULL != pScript)) {
                    printf("sccript != NULL ready to init()\n");
                    ret = pScript->init();  //  初始化一个执行step链表，存放在pScript->m_script中；
                    printf("ret = %d\n", ret);
                }
                if (protocol == "TCP") {  //tcp
                    pScript->m_protocol = "TCP";
                    createSocketPool();
                    for (int i = 0; i < m_threadPool.size(); ++i) {  //init connect pool,the number is the same as the number of thread pool
                        string connStr = pScript->getConnStr();
                        std::cout << connStr << std::endl;
                        CSocket *cSocket = NULL;
                        if (csocket_map.size() > 0) {
                            cSocket = csocket_map[connStr]->getSocket();  //got the socket connect,the number of connections is the same as the number of threads
                        }
                        if (cSocket != NULL) {
                            m_threadPool.initConnect(i, pScript, cSocket);
                        }
                    }
                } else if (protocol == "HTTP") {  //http
                    pScript->m_protocol = "HTTP";
                    for (int i = 0; i < m_threadPool.size(); ++i)  //init connect pool,the number is the same as the number of thread pool
                            {
                        m_threadPool.initHttpConnect(i);
                    }
                }

                if (likely(ret)) {
                    vector<int> workers;
                    int nId = chooseWorkers(workers);
                    if (unlikely(nId < 0)) {
                        evhttp_send_reply(req, HTTP_INTERNAL, "Internal Error",
                        NULL);
                    } else {
                        if (0 == nId) {
                            printf("nID==0 and pool size= %d\n", m_threadPool.size());
                            for (int i = 0; i < m_threadPool.size(); ++i) {
                                TCmdStartEvent *pCmd = new TCmdStartEvent(pScript, 10, 50, 0);
                                m_threadPool.order(pCmd, i);  //  worker thread depends on event;
                            }
                        } else {
                            printf("nID=%d\n", nId);
                            for (vector<int>::iterator iter = workers.begin(); iter != workers.end(); ++iter) {
                                TCmdStartEvent *pCmd = new TCmdStartEvent(pScript, 1, 25, 0);
                                m_threadPool.order(pCmd, *iter);
                            }
                        }
                        evbuffer_add(buff, "start task successfully!", 24);
                        evhttp_send_reply(req, HTTP_OK, "OK", buff);
                    }
                } else {
                    evbuffer_add(buff, "load task failed!", 17);
                    evhttp_send_reply(req, HTTP_INTERNAL, "Internal Error",
                    NULL);
                }
            }
        }
    } else {  //other url
        evhttp_send_reply(req, HTTP_NOTFOUND, "Not Found", NULL);
    }
    evbuffer_free(buff);
    if (likely(NULL != url)) {
        free(url);
    }
    return;
}

int TManager::chooseWorkers(vector<int> & workers) {
//TODO
    return 0;
}

void httpd_cb(struct evhttp_request *req, void *arg) {
    ((TManager*) arg)->processHttpReq(req);
}

void httpd_ptf_cb(struct evhttp_request *req, void *arg) {
    const char * str = evhttp_request_get_host(req);
    const char *decode_uri = evhttp_request_get_uri(req);
    evbuffer *buff = evbuffer_new();
    evbuffer_add(buff, "Welcome to mpt!", 15);
    printf("[%s][%s]\n", str, decode_uri);
    evhttp_send_reply(req, HTTP_OK, "OK", buff);
    evbuffer_free(buff);
}

void TManager::createSocketPool() {
    csocket_map["127.0.0.1:8000"] = new CSocketPool(20, "127.0.0.1:8000");
}
