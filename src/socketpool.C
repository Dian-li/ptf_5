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
#include "socketpool.h"

/**
 * CSocket
 */
CSocket::CSocket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_port = 8000;
    server_ip="127.0.0.1";
}
CSocket::~CSocket()
{
    close(sockfd);
    if(ev_cmd!= NULL)
    {
        event_free(ev_cmd);
        ev_cmd = NULL;
    }
    if(readbuff!= NULL)
    {
        evbuffer_free(readbuff);
        readbuff = NULL;
    }
    if(writebuff!=NULL)
    {
        evbuffer_free(writebuff);
        writebuff = NULL;
    }
}
CSocket::CSocket(bool tcp,char* server_ip,uint16_t server_port,struct event_base * base)
{
    if(tcp)
    {
        s_base = base;
        writebuff = evbuffer_new();
        readbuff = evbuffer_new();
        sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        printf("sockfd:%d ",sockfd);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, server_ip, &servaddr.sin_addr);
        //servaddr.sin_addr.s_addr = htonl(server_ip);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
        servaddr.sin_port = htons(server_port);//设置的端口为DEFAULT_PORT
        printf("%s connected！\n",inet_ntoa(servaddr.sin_addr));
        evutil_make_socket_nonblocking(sockfd);
        bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
        bufferevent_socket_connect(bev, (struct sockaddr*)&servaddr, sizeof(servaddr));
        bufferevent_setcb(bev, read_cb, write_cb, error_cb,(void *)this);
        bufferevent_enable(bev, EV_READ|EV_PERSIST);
        //connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    }else
    {
        sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    }
}

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

char* CSocket::recvMsg(int sockfd)
{
    if(sockfd>0)
    {
        if(s_base==NULL)
        {
            printf("base is null,please create a new one!\n");
            return false;
        }
        size_t bufflen = 0;
        if(readbuff!=NULL)
        {
            bufflen = evbuffer_get_length(readbuff);
        }

        ev_uint32_t record_len;
        char * read = NULL;
        if(bufflen>0)
        {
            record_len = ntohl(bufflen);
            read = (char*)malloc(record_len);
            if(read == NULL)
                return NULL;
            evbuffer_remove(readbuff,read,record_len);
            printf("read from the buffer:%s\n",read);

        }
        else
        {
            printf("buffer is empty\n");
            read="";
        }
        return read;
    }
    else
    {
        return "socket id is incorrect\n";
    }

}
bool CSocket::sendMsg(char *bytes,int sockfd)
{
    if(s_base==NULL)
    {
        printf("base is null,please create a new one!\n");
        return false;
    }
    if(writebuff !=NULL)
    {
        size_t bufflen = evbuffer_get_length(writebuff);
        evbuffer_drain(writebuff,bufflen);//remove datas from buffer
    }
    evbuffer_add(writebuff,bytes, strlen(bytes));
    size_t ret = bufferevent_write_buffer(bev,writebuff);
    //size_t ret = bufferevent_write(bev,bytes,strlen(bytes));
    printf("write data to buffer:%s\n",bytes);
    if(ret)
        return true;
    else
        return false;
}

void CSocket::read_cb(struct bufferevent *bev, void *arg)
{
    CSocket * cSocket = (CSocket*)arg;
    size_t bufflen = evbuffer_get_length(cSocket->readbuff);
    int len = bufferevent_read_buffer(bev, cSocket->readbuff);
    printf("[read callback]buffer's result is:%d\n",len);
}

void CSocket::write_cb(struct bufferevent *bev, void *arg)
{
    CSocket * cSocket = (CSocket*)arg;
    int bufflen = evbuffer_get_length(cSocket->writebuff);
    printf("[write callback]buffer's length is:%d\n",bufflen);
}

void CSocket::error_cb(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("Some other error.\n");
    }
    else if (event & BEV_EVENT_CONNECTED) {
        printf("Client has successfully cliented.\n");
        return;
    }
    //bufferevent_free(bev);
}

void CSocket::send_cb(int fd, short events, void *arg)
{
    printf("yes\n");
    struct evbuffer * buff = (struct evbuffer*)arg;
    int ret = evbuffer_write(buff,fd);
    if(ret==0)
        printf("send data success!\n");
    else
        printf("send data error!\n");
}


/**
 * CSocketPool
 */
CSocketPool::CSocketPool()
{
    base = event_base_new();
    c_sCount=0;
    m_queue=NULL;
}
CSocketPool::CSocketPool(int count,const char* ipAndPort)
{
    base = event_base_new();
    c_sCount = count;
    //struct bufferevent* bev = bufferevent_socket_new(base,-1,BEV)
    m_queue = new TRingbuffer<CSocket>(count);
    char temp[20] = {'0'};
    strcpy(temp,ipAndPort);
    char* ip = strtok(temp,":");
    int port = atoi(strtok(NULL,":"));
    for(int i=0;i<count;i++)
    {
        CSocket * cSocket = new CSocket(true,ip,port,base);
        m_queue->enqueue(cSocket);
    }
    printf("queue size if:%d\n",m_queue->size());
    pthread_t pthread;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    int ret = pthread_create(&pthread,&attr,CSocketPool::loop_event,(void*)base);
    if(ret!=0)
    {
        printf("Can't creat thread!\n");
    }
}

CSocketPool::~CSocketPool()
{
    //delete(m_queue);
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
    if(socket!=NULL)
    {
        m_queue->enqueue(socket);
    }
}

void* CSocketPool::loop_event(void *arg)
{
    struct event_base* base = (struct event_base*)arg;
    event_base_dispatch(base);
}

int CSocketPool::getSize()
{
    return c_sCount;
}

void CSocketPool::destroy()
{
    //delete m_queue;
}




