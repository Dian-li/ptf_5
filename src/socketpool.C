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

#include "socketpool.h"


CSocket::CSocket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ssl=false;
    server_port = 8000;
    server_ip="127.0.0.1";
}
CSocket::~CSocket()
{
    close(sockfd);
}
CSocket::CSocket(bool tcp,char* server_ip,uint16_t server_port)
{
    if(tcp)
    {
        sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        printf("sockfd:%d\n",sockfd);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, server_ip, &servaddr.sin_addr);
        //servaddr.sin_addr.s_addr = htonl(server_ip);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
        servaddr.sin_port = htons(server_port);//设置的端口为DEFAULT_PORT
        printf("%s\n",inet_ntoa(servaddr.sin_addr));
        connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    }else
    {
        sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    }
}
bool CSocket::IsSSL() const { return false;}
void CSocket::SetSSL(bool ssl) {}

int CSocket::getSocketFd()
{
    return sockfd;
}
char* CSocket::getIP()
{
    return server_ip;
}
uint16_t CSocket::getPort()
{
    return server_port;
}

char* CSocket::recvMsg()
{
    if(sockfd>0)
    {
        int rec_len = recv(sockfd, recvbuff, MAXLINE,0);
        if(rec_len!= -1){
            recvbuff[rec_len] = '\0';
            return recvbuff;
        }else{
            return NULL;
        }
    }
}
bool CSocket::sendMsg(char *bytes)
{
    int len = send(sockfd, bytes, strlen(bytes), 0);
    if(len>0)
        return true;
    else
        return false;
}


CSocketPool::CSocketPool()
{
    c_sCount=0;
    m_queue=NULL;
}
CSocketPool::CSocketPool(int count,const char* ipAndPort)
{
    c_sCount = count;
    m_queue = new TRingbuffer<CSocket>(count);
    char temp[20] = {'0'};
    strcpy(temp,ipAndPort);
    char* ip = strtok(temp,":");
    int port = atoi(strtok(NULL,":"));
    while(count--)
    {
        CSocket * cSocket = new CSocket(true,ip,port);
        m_queue->enqueue(cSocket);
        //delete cSocket;
    }
}

CSocketPool::~CSocketPool()
{
    delete(m_queue);
}

CSocket* CSocketPool::getSocket()
{
    CSocket* CSocket = m_queue->dequeue();
    return CSocket;
}
int CSocketPool::getSocket(uint32_t MilliSeconds) const
{
    return -1;
}
int CSocketPool::getSocket(char* server_ip, uint16_t server_port) const { return -1;}
int CSocketPool::getSocket(char* server_ip, uint16_t server_port, uint32_t MilliSeconds) const { return -1;}
void CSocketPool::backSocket(CSocket* socket)
{
    m_queue->enqueue(socket);
}


int CSocketPool::getSize()
{
    return c_sCount;
}

void CSocketPool::destroy()
{
    delete m_queue;
}



