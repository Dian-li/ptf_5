//
// Created by dian on 17-10-16.
//

#ifndef PTF_5_SOCKETPOOL_H
#define PTF_5_SOCKETPOOL_H


#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <mutex>
#include <unordered_map>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>
#include <vector>
#include <set>
#include <pthread.h>
#include <queue>

#include "timeoutqueue.h"
#include "protocol.h"
#define MAXLINE 4096

using std::queue;
class TTransation;
class TimeoutQueue;
class Connection{
public:
	Connection();
	~Connection();
};
class CSocket: public Connection
{
public:
    CSocket();
    CSocket(bool tcp,char* server_ip,uint16_t server_port,struct event_base * base); // 创建一个SOCKET，如果tcp为true，则创建TCP套接字，否则创建UDP套接字
    ~CSocket();
    int getSocketFd();
    char* getIP();
    char* recvMsg(int sockfd);
    bool sendMsg(char* bytes,int sockfd);
    uint16_t getPort();
    void setTransation(TTransation *transation);
    void setTimeoutQueue(TimeoutQueue* queue);
    TTransation* getTTransation();
    TimeoutQueue* getTimeoutQueue();
public:
    TAppProtocolTable* tAppProtocolTable;
    int index;
private:
    int     sockfd;
    struct  evbuffer * readbuff;
    struct  evbuffer * writebuff;//not use
    struct  bufferevent *bev;
    struct  event * ev_cmd;
    struct  event_base * s_base;
    uint16_t server_port; // 服务端的监听端口
    char   *server_ip; // 服务端监听的IP
    struct sockaddr_in    servaddr;
    TTransation *tTransation;
    TimeoutQueue* timeoutQueue;

    static void read_cb(struct bufferevent *bev, void *arg); //读
    static void write_cb(struct bufferevent *bev, void *arg); //写
    static void error_cb(struct bufferevent *bev, short event, void *arg);//错误
    static void send_cb(int fd, short events, void *arg);
    static void buffer_add_cb(struct evbuffer *buffer, const struct evbuffer_cb_info *info, void *arg);//buffer added callback
};

class CHttpConnection: public Connection{
public:
	struct event_base * m_httpbase;
	TTransation * m_tTransation;
	struct evhttp_request * m_request;
	struct evdns_base* m_dnsbase;
	struct evhttp_connection* m_connection;
	CHttpConnection(struct event_base * m_httpbase);
	~CHttpConnection();
};

class CSocketPool
{
public:
    CSocketPool();
    CSocketPool(int count,const char* ipAndPort);
    ~CSocketPool();
    int getSize();
    void destroy(); // 断开所有连接，并销毁所有CTcpClient
    CSocket* getSocket();
    void backSocket(CSocket* socket);
    int getSocket(uint32_t MilliSeconds) const ;
    int getSocket(char* server_ip,uint16_t server_port) const; // 返回句柄值
    int getSocket(char* server_ip,uint16_t server_port,uint32_t MilliSeconds)const ;

private:
    int c_sCount;
    queue<CSocket*>  m_queue;
    struct event_base *base;
    std::mutex poolmutex;
    static void* loop_event(void* arg);
    TimeoutQueue* timeoutQueue;
};

//static std::unordered_map<std::string, CSocketPool*>  csocket_map;
//static std::mutex                                csocket_map_mutex;

#endif //PTF_5_SOCKETPOOL_H
