//
// Created by dian on 17-11-20.
//

#include "httpstep.h"
#include <map>
#include <iostream>
#include "httpevent.h"

using std::map;

THTTPStep::THTTPStep() {

}

THTTPStep::~THTTPStep() {
}

bool THTTPStep::init(xmlNodePtr pNode) {
	xmlChar* pValue = NULL;
	bool ret = TStep::init(pNode);
	if (unlikely(!ret))
		return ret;
	m_protocol = P_HTTP;
	xmlNodePtr pChildNode = pNode->xmlChildrenNode;
	while (NULL != pChildNode) {
		if (xmlNodeIsText(pChildNode)
				|| XML_CDATA_SECTION_NODE == pChildNode->type) {
			pChildNode = pChildNode->next;
			continue;
		}
		char * node_name = (char *) pChildNode->name;
		char * node_value = (char *) xmlNodeGetContent(pChildNode);
		//printf("[%s:", node_name);
		//printf("%s]\n", node_value);
		if (string(node_name) == "wait") {
			if (NULL != node_value) {
				//printf("time delay:%s s\n", node_value);
				m_timer = atoi((const char*) node_value);
			}
		} else if (string(node_name) == "host") {
			if (NULL != node_value) {
				m_host = (const char*) node_value;
			} else {
				printf("TTCPStep init() get destination host address failed !\n");
				return false;
			}
		} else if (string(node_name) == "url") {
			if (NULL != node_value) {
				m_url = (const char*) node_value;
			} else {
				printf("THTTPStep init() get url failed !\n");
				return false;
			}
		}else if(string(node_name) == "header"){
			if(NULL !=node_value){
				m_header =  (const char*) node_value;
			}else{
				printf("THTTPStep init() get header failed !\n");
				return false;
			}
		}else if(string(node_name) == "body"){
			if(NULL != node_value){
				m_body =  (const char*) node_value;
			}else{
				m_body = "";
			}
		}
		pChildNode = pChildNode->next;
	}
//	if (m_type == ST_SEND) {
//		m_tmptEvent = new THTTPEvent(ET_HTTP, ED_RX);
//	} else if (m_type == ST_RECV) {
//		m_tmptEvent = new THTTPEvent(ET_HTTP, ED_TX);
//	}
	return true;
}

void THTTPStep::supsend() {

}

void THTTPStep::suprecv() {

}

TStepResult THTTPStep::send(TMPTEvent *pEvent) {
	printf("send step\n");
	pthread_t thread_id = pthread_self();
	CHttpConnection* cHttpConn = NULL;
	cps_map_mutex.lock();
	cHttpConn =(CHttpConnection*)cps_map[thread_id];
    cps_map_mutex.unlock();
    if(NULL == cHttpConn){
    	return SRST_ERREND;
    }
    m_base = cHttpConn->m_httpbase;
	struct evdns_base* m_dnsbase = evdns_base_new(m_base,1);
	struct evhttp_request * m_request = evhttp_request_new(ReadCallback,cHttpConn);
	evhttp_request_set_error_cb(m_request,ErrorCallback);
	cHttpConn->m_dnsbase = m_dnsbase;
	cHttpConn->m_request = m_request;
	m_host = "http://"+m_host;
	struct evhttp_uri * uri = evhttp_uri_parse(m_host.c_str());
	const char* host;
	int port;
	if(uri != nullptr){
		host = evhttp_uri_get_host(uri);
		port = evhttp_uri_get_port(uri);
		if(port<=0) port = 80;
	}else{
		host = m_host.c_str();
		port = 80;
	}
	//std::cout<<"host is "<<host<<" ,port is "<< port<<std::endl;
	m_connection = cHttpConn->m_connection;
	if(m_connection == NULL){
		cHttpConn->m_connection = evhttp_connection_base_new(m_base,m_dnsbase,host,port);
		m_connection = cHttpConn->m_connection;
	}
	evhttp_connection_set_closecb(m_connection,ClosedCallback,cHttpConn);
	//std::cout<<"connection"<<m_connection<<std::endl;
	//std::cout<<"request"<<m_request<<std::endl;
	if(!m_connection){
		printf("connection has errors!");
		return SRST_ERREND;
	}
	struct evkeyvalq* req_header = evhttp_request_get_output_headers(m_request);
	httpparser* httpParser = new httpparser();
	httpParser->parse(m_header);
	map<string,string> m = httpParser->m_headers;
	for(map<string,string>::iterator it = m.begin();it!=m.end();it++){
		evhttp_add_header(req_header,(it->first).c_str(),(it->second).c_str());
	}
	struct evbuffer* outputbuffer = evhttp_request_get_output_buffer(m_request);
	evbuffer_add(outputbuffer,m_body.c_str(),m_body.size());
	if( (httpParser->m_type).size()!= 0 && (httpParser->m_url).size() != 0){
		if(httpParser->m_type == "GET"){
			evhttp_make_request(m_connection,m_request,EVHTTP_REQ_GET,(httpParser->m_url).c_str());//url encoded未解决
		}else if(httpParser->m_type == "POST"){
			evhttp_make_request(m_connection,m_request,EVHTTP_REQ_POST,(httpParser->m_url).c_str());
		}
	}else{
		return SRST_ERREND;
	}
	event_base_dispatch(m_base);
	printf("send is successful!\n");
	return SRST_SUCC;
}

TStepResult THTTPStep::recv(TMPTEvent *pEvent) {
	printf("recv step\n");
	THttpEvent* thttpevent = (THttpEvent*)pEvent;
	int code = thttpevent->response_code;
	if(code == 0){
		return SRST_ERREND;
	}
	if(code == HTTP_OK || code == 302 || code == 404){//暂时放宽条件
		printf("200 OK!\n");
		return SRST_SUCC;
	}else{
		return SRST_ERREND;
	}
}

void THTTPStep::ReadCallback(struct evhttp_request *req,void* arg){
	printf("request sending complete or error occors!\n");
	if(req == NULL) return;
	THttpEvent*  thttpevent = new THttpEvent(ED_TX,req);
	thttpevent->response_code = evhttp_request_get_response_code(req);
	CHttpConnection* cHttpConn = (CHttpConnection*)arg;
	TTransation* ttransation = cHttpConn->m_tTransation;
	ttransation->pushEvent(thttpevent);
	event_base_loopexit((struct event_base*)cHttpConn->m_httpbase, NULL);
}

void THTTPStep::ErrorCallback(evhttp_request_error req_error,void* arg){
	printf("request error\n");
	CHttpConnection* cHttpConn = (CHttpConnection*)arg;
	event_base_loopexit((struct event_base*)cHttpConn->m_httpbase, NULL);
	switch(req_error){
	case EVREQ_HTTP_TIMEOUT:
		printf("EVREQ_HTTP_TIMEOUT\n");
		break;
	case EVREQ_HTTP_EOF:
		printf("EVREQ_HTTP_EOF\n");
		break;
	case EVREQ_HTTP_INVALID_HEADER:
	   printf("EVREQ_HTTP_INVALID_HEADER\n");
	   break;
	case EVREQ_HTTP_BUFFER_ERROR:
	   printf("EVREQ_HTTP_BUFFER_ERROR\n");
	   break;
	case EVREQ_HTTP_REQUEST_CANCEL:
		printf("EVREQ_HTTP_REQUEST_CANCEL\n");
		break;
	case EVREQ_HTTP_DATA_TOO_LONG:
		printf("EVREQ_HTTP_DATA_TOO_LONG\n");
		break;
	default:
		printf("OTHER ERRORS\n");
	}
}

void THTTPStep::ClosedCallback(struct evhttp_connection* connection, void* arg){
	printf("connection closed\n");
	CHttpConnection* cHttpConn = (CHttpConnection*)arg;
	event_base_loopexit((struct event_base*)cHttpConn->m_httpbase, NULL);
	if(cHttpConn->m_dnsbase != NULL){
		evdns_base_free(cHttpConn->m_dnsbase,0);
		cHttpConn->m_dnsbase = NULL;
	}
}

