//
// Created by dian.
//

#ifndef PTF_5_CSOCKET_H
#define PTF_5_CSOCKET_H

class CSocket
{
public:
    CSocket();
    bool create(bool tcp); // 创建一个SOCKET，如果tcp为true，则创建TCP套接字，否则创建UDP套接字
    bool IsSSL() const; // 如果返回true，则表示支持SSL，否则不支持
    void SetSSL(bool ssl); // 更新是否支持SSL标识
private:
    bool m_ssl; // 否支持SSL标识
};

/**
 * tcp connector
 */
class CTcpConnector: public CSocket
{
public:
    int receive(const char* buf, uint32_t buflen); // 接收数据
    int send(char* buf, uint32_t buflen); // 发送数据
};

/**
 * tcp client
 */
class CTcpClient: public CSocket//tcp client
{
public:
    bool TimedWait(uint32_t MilliSeconds,const char* ServIP, uint16_t ServPort, const char* LocalIP=NULL,uint16_t LocalPort=0); // 带超时功能的连接，支持绑定本地IP和端口
};

typedef struct//tcp connection struct
{
    bool ssl; // 是否为SSL连接
    uint16_t server_port; // 服务端的监听端口
    uint16_t local_port; // 需要绑定的本地端口
    std::string server_ip; // 服务端监听的IP
    std::string local_ip; // 需要绑定的本地IP
}TConnectParam;

class CTcpClientManager//tcp client manager
{
public:
    void add(const TConnectParam&); // 添加一个连接
    bool create(); // 执行所有连接
    void destroy(); // 断开所有连接，并销毁所有CTcpClient
};

/**
 * tcp listener
 */
typedef struct//监听参数结构体
{
    bool ssl; // 是否作为一个SSL类型的监听者
    uint16_t port; // 监听端口
    std::string ip; // 监听IP地址
}TListenParam;

typedef std::list<TListenParam*> TListenParamList;//监听参数结构体链表

class CTcpListener: public CSocket//tcp listener
{
public:
    bool listen(const char* ip, uint16_t port); // 监听
    int accept(); // 接受一个连接，并返回连接的句柄
};

template <class CListenerClass>//监听管理类
class CTcpListnerManager
{
public:
    void add(TListenParam&); // 添加一个监听
    bool create(); // 创建所有监听
    void destroy(); // 销毁所有监听
private:
    TListenParamList m_lstListenParam; // 监听参数表
};

/**
 * Servant
 */
class CServant
{
public:
    void attach(int fd); // 将CServant关联到一个fd上。
};

class CServantPool//servant pool
{
public:
    bool create(uint32_t size); // 创建Servant池，size为池中Servant个数
    void destroy(); // 释放所有Servant
    CServant* borrow(); // 从池中借出一个Servant
    void reclaim(CServant* pServant); // 将一个Servant还回到池中
private:
    CArrayQueue<CServant*> m_ServantQueue;
};

/**
 * ssl manager
 */
class CSSLmanager
{
public:
    bool SSLinit();
    void SSLfini();
};

/**
 * timeout manager
 */
class CTimeoutable
{
public:
    time_t GetTimestamp() const; // 返回时间戳
    void UpdateTimestamp(time_t timestamp); // 更新时间戳
private:
    time_t m_timestamp; // 时间戳
};

class ITimeoutEvent
{
public:
    virtual void OnTimeout(CTimeoutable*) = 0;
};

class CTimeoutManager
{
public:
    void CheckTimeout(); // 检测超时的连接，如果超时，则将超时的从超时队列中删除，并回调ITimeoutEvent方法，以便将超时的连接关闭掉
    void add(CTimeoutable*); // 添加一个连接到超时队列中
    void remove(CTimeoutable*); // 从超时队列中删除一个连接
};

#endif //PTF_5_CSOCKET_H
