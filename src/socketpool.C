//
// Created by dian on 17-10-16.
//

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <event2/util.h>
#include <stdlib.h>
#include <event2/dns.h>

#include "mptevent.h"
#include "socketpool.h"
#include "tcpevent.h"
#include "transaction.h"

Connection::Connection(){}

Connection::~Connection(){}


/**
 * CSocket
 */
CSocket::CSocket() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_port = 8000;
	server_ip = "127.0.0.1";
}

CSocket::~CSocket() {
	close(sockfd);
	if (ev_cmd != NULL) {
		event_free(ev_cmd);
		ev_cmd = NULL;
	}
	if (readbuff != NULL) {
		evbuffer_free(readbuff);
		readbuff = NULL;
	}
	if (writebuff != NULL) {
		evbuffer_free(writebuff);
		writebuff = NULL;
	}
	if (timeoutQueue != NULL) {
		delete (timeoutQueue);
		timeoutQueue = NULL;
	}
}

CSocket::CSocket(bool tcp, char* server_ip, uint16_t server_port,
		struct event_base * base) {
	if (tcp) {
		s_base = base;
		index = 0;
		tAppProtocolTable = new TAppProtocolTable();
		writebuff = evbuffer_new();
		readbuff = evbuffer_new();
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		printf("sockfd:%d ", sockfd);
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton(AF_INET, server_ip, &servaddr.sin_addr);
		//servaddr.sin_addr.s_addr = htonl(server_ip);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
		servaddr.sin_port = htons(server_port); //设置的端口为DEFAULT_PORT
		printf("%s connected！\n", inet_ntoa(servaddr.sin_addr));
		evutil_make_socket_nonblocking(sockfd);
		bev = bufferevent_socket_new(base, sockfd,
				BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
		bufferevent_socket_connect(bev, (struct sockaddr*) &servaddr,
				sizeof(servaddr));
		bufferevent_setcb(bev, read_cb, write_cb, error_cb, (void *) this);
		evbuffer_add_cb(readbuff, buffer_add_cb, (void *) this);
		bufferevent_enable(bev, EV_READ | EV_PERSIST); //注册读事件
		//connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	} else {
		sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
}

int CSocket::getSocketFd() {
	return sockfd;
}

char* CSocket::getIP() {
	return server_ip;
}

uint16_t CSocket::getPort() {
	return server_port;
}

char* CSocket::recvMsg(int sockfd) {
	if (sockfd > 0) {
		if (s_base == NULL) {
			printf("base is null,please create a new one!\n");
			return false;
		}
		size_t bufflen = 0;
		if (readbuff != NULL) {
			bufflen = evbuffer_get_length(readbuff);
		}

		ev_uint32_t record_len;

	} else {
		return "socket id is incorrect\n";
	}
}

bool CSocket::sendMsg(char *bytes, int sockfd) {
	if (s_base == NULL) {
		printf("base is null,please create a new one!\n");
		return false;
	}
	if (writebuff != NULL) {
		size_t bufflen = evbuffer_get_length(writebuff);
		evbuffer_drain(writebuff, bufflen);        //remove datas from buffer
	}
	//
	evbuffer_add(writebuff, bytes, strlen(bytes));
	size_t ret = bufferevent_write_buffer(bev, writebuff);        //触发写事件
	//size_t ret = bufferevent_write(bev,bytes,strlen(bytes));
	//printf("write data to buffer:%s\n", bytes);
	//printf("ret = %d\n", ret);
	if (ret)
		return true;
	else
		return false;
}

void CSocket::read_cb(struct bufferevent *bev, void *arg) {	//read
	printf("exe read_cb\n");
	CSocket * cSocket = (CSocket*) arg;
	TTransation* transation = cSocket->tTransation;
	bufferevent_lock(bev);
	struct evbuffer* input = bufferevent_get_input(bev);//新增数据缓冲区
	int input_len = evbuffer_get_length(input);
	//printf("input buffer is %d\n", input_len);
	evbuffer_lock(input);
	evbuffer_remove_buffer(input,cSocket->readbuff,input_len-1);
	evbuffer_drain(input,evbuffer_get_length(input));//清空输入缓存
	evbuffer_unlock(input);
	bufferevent_unlock(bev);
	int record_len2 = evbuffer_get_length(cSocket->readbuff);

	//printf("the sum of the buffer %d bytes\n", record_len2);
}

void CSocket::write_cb(struct bufferevent *bev, void *arg) {
	CSocket * cSocket = (CSocket*) arg;
	int bufflen = evbuffer_get_length(cSocket->writebuff);
	//printf("[write callback]send is success\n");
}

void CSocket::error_cb(struct bufferevent *bev, short event, void *arg) {
	CSocket * cSocket = (CSocket*) arg;
	TTCPEvent* errorEvent = new TTCPEvent();
	errorEvent->setType(ET_ERROR);
	if (event & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (event & BEV_EVENT_ERROR) {
		printf("Some other error.\n");
	} else if (event & BEV_EVENT_CONNECTED) {
		printf("Client has successfully cliented.\n");
		return;
	}
	cSocket->tTransation->pushEvent(errorEvent);    //error

	//bufferevent_free(bev);
}

void CSocket::buffer_add_cb(struct evbuffer *buffer,
		const struct evbuffer_cb_info *info, void *arg) {
	CSocket * cSocket = (CSocket*) arg;
	if (info->n_added > 0) {
		struct evbuffer* copybuffer = evbuffer_new();
		evbuffer_lock(buffer);
		evbuffer_lock(copybuffer);
		size_t buffer_len = evbuffer_get_length(buffer);
		char * tmp_array = new char[buffer_len];
		evbuffer_copyout(buffer,tmp_array,buffer_len);//拷贝读缓存中数据到临时数组
		evbuffer_add(copybuffer,tmp_array,buffer_len);//添加数据至临时缓存
		evbuffer_unlock(buffer);//释放锁
		delete tmp_array;
		tmp_array = NULL;
		/*	size_t len11 = evbuffer_get_length(buffer);
		 size_t len2 = evbuffer_get_length(copybuffer);
		 printf("after copy copybuffen length is %d\n",len2);*/
		int found = cSocket->tAppProtocolTable->receiveHttpResponse(
				copybuffer);//处理数据
		evbuffer_free(copybuffer);
		copybuffer = NULL;
		char * message = NULL;
		if (found == -1) {
			return;
		} else {
			//printf("found is %d\n", found);
			int ret = evbuffer_drain(buffer,found);
			//message = (char*) malloc(found + 1);
			//evbuffer_remove(buffer, message, found);
			//printf("[read callback]buffer's result is:%s\n", message);
			TTCPEvent *tcpEvent = new TTCPEvent();
			tcpEvent->setType(ET_TCPDATA);
			//tcpEvent->setData(found);
			if(ret == 0){
				tcpEvent->setLength(found);
			}else{
				tcpEvent->setLength(-1);
			}

			cSocket->tTransation->pushEvent(tcpEvent);    //发送数据
			printf("send event success!\n");
		}
//		if (message != NULL) {
//			free(message);
//			message = NULL;
//		}

	}
}

void CSocket::setTransation(TTransation *transation) {
	this->tTransation = transation;
}

void CSocket::setTimeoutQueue(TimeoutQueue *queue) {
	this->timeoutQueue = queue;
}

void CSocket::send_cb(int fd, short events, void *arg) {
	struct evbuffer * buff = (struct evbuffer*) arg;
	int ret = evbuffer_write(buff, fd);
	if (ret == 0)
		printf("send data success!\n");
	else
		printf("send data error!\n");
}

TTransation* CSocket::getTTransation() {
	return tTransation;
}

TimeoutQueue* CSocket::getTimeoutQueue() {
	return timeoutQueue;
}

CHttpConnection::CHttpConnection(struct event_base * base){
	this->m_httpbase = base;
}

CHttpConnection::~CHttpConnection(){
	if(NULL != m_connection){
		evhttp_connection_free(m_connection);
		m_connection = NULL;
	}
	if(NULL != m_dnsbase){
		evdns_base_free(m_dnsbase,0);
		m_dnsbase = NULL;
	}
}

/**
 * CSocketPool
 */
CSocketPool::CSocketPool() {
	base = event_base_new();
	timeoutQueue = new TimeoutQueue();
	c_sCount = 0;
}

CSocketPool::CSocketPool(int count, const char* ipAndPort) {
	base = event_base_new();
	c_sCount = count;
	//struct bufferevent* bev = bufferevent_socket_new(base,-1,BEV)
	timeoutQueue = new TimeoutQueue();
	char temp[20] = { '0' };
	strcpy(temp, ipAndPort);
	char* ip = strtok(temp, ":");
	int port = atoi(strtok(NULL, ":"));
	for (int i = 0; i < count; i++) {
		CSocket * cSocket = new CSocket(true, ip, port, base);
		cSocket->setTimeoutQueue(timeoutQueue);
		m_queue.push(cSocket);
		std::cout << "[CSocket init:]" << cSocket << std::endl;
	}
	pthread_t pthread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int ret = pthread_create(&pthread, &attr, CSocketPool::loop_event,
			(void*) base);
	if (ret != 0) {
		printf("Can't creat thread!\n");
	}

	//timeoutQueue->loop();    //loop for wait
}

CSocketPool::~CSocketPool() {
}

CSocket* CSocketPool::getSocket() {
	poolmutex.lock();
	CSocket* cSocket = m_queue.front();
	m_queue.pop();
	poolmutex.unlock();
	std::cout << "[CSocketpool dequeue]:" << cSocket << std::endl;
	std::cout << "[CSocketpool size]:" << getSize() << std::endl;
	return cSocket;
}
int CSocketPool::getSocket(uint32_t MilliSeconds) const {
	return -1;
}
int CSocketPool::getSocket(char* server_ip, uint16_t server_port) const {
	return -1;
}
int CSocketPool::getSocket(char* server_ip, uint16_t server_port,
		uint32_t MilliSeconds) const {
	return -1;
}
void CSocketPool::backSocket(CSocket* socket) {
	if (socket != NULL) {
		poolmutex.lock();
		m_queue.push(socket);
		poolmutex.unlock();
	}
}

void* CSocketPool::loop_event(void *arg) {
	struct event_base* base = (struct event_base*) arg;
	event_base_dispatch(base);
}

int CSocketPool::getSize() {
	return m_queue.size();
}

void CSocketPool::destroy() {
	//delete m_queue;
}

