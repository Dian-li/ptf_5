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


#include "ringbuff.h"

#define MAXLINE 4096

class CSocket
{
public:
    CSocket();
    CSocket(bool tcp,char* server_ip,uint16_t server_port); // 创建一个SOCKET，如果tcp为true，则创建TCP套接字，否则创建UDP套接字
    ~CSocket();
    bool IsSSL() const; // 如果返回true，则表示支持SSL，否则不支持
    void SetSSL(bool ssl); // 更新是否支持SSL标识
    int getSocketFd();
    char* getIP();
    char* recvMsg();
    bool sendMsg(char* bytes);
    uint16_t getPort();

private:
    bool ssl; // 否支持SSL标识
    int sockfd;
    char recvbuff[4096];//接收缓冲区
    uint16_t server_port; // 服务端的监听端口
    char* server_ip; // 服务端监听的IP
    struct sockaddr_in    servaddr;
};


class CSocketPool
{
public:
    CSocketPool();
    CSocketPool(int count,const char* ipAndPort);
    ~CSocketPool();
    int getSize();
    //void add(const CHandle&); // 添加一个连接
    void destroy(); // 断开所有连接，并销毁所有CTcpClient
    CSocket* getSocket();
    void backSocket(CSocket* socket);
    int getSocket(uint32_t MilliSeconds) const ;
    int getSocket(char* server_ip,uint16_t server_port) const; // 返回句柄值
    int getSocket(char* server_ip,uint16_t server_port,uint32_t MilliSeconds)const ;
private:
    int c_sCount;
    TRingbuffer<CSocket> *m_queue;
};

static std::unordered_map<std::string, CSocketPool*>  csocket_map;
static std::mutex                                csocket_map_mutex;

#endif //PTF_5_SOCKETPOOL_H
