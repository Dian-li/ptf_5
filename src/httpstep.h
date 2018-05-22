//
// Created by dian on 17-11-20.
//

#ifndef PTF_5_HTTPSTEP_H
#define PTF_5_HTTPSTEP_H

#include "step.h"
#include "mptthreadpool.h"
#include "socketpool.h"
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include "httpparser.h"
#include "mptconst.h"

class THTTPStep: public TStep {
public:
	THTTPStep();

	~THTTPStep();

	bool init(xmlNodePtr pNode);

	TStepResult send(TMPTEvent *pEvent);

	TStepResult recv(TMPTEvent *pEvent);

    void supsend();

    void suprecv();

    TTransation *m_tTransation;


private:

	//string m_connStr;
	string m_host;
	string m_url;
	string m_header;
	string m_body;
	struct event_base* m_base;
	struct evhttp_connection* m_connection;

	static void ReadCallback(struct evhttp_request* req,void* arg);
	static void ClosedCallback(struct evhttp_connection* connection, void* arg);
	static void ErrorCallback(evhttp_request_error req_error,void * arg);
};

#endif //PTF_5_HTTPSTEP_H
