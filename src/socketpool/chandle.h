//
// Created by dian.
//

#ifndef PTF_5_CHANDLE_H
#define PTF_5_CHANDLE_H

class CHandle
{
public:
    CHandle();
    SetHandle(int fd); // 设置句柄
    int GetHandle() const; // 返回句柄值
    THandleType GetType() const; // 得到句柄对象类型
    bool SetNonBlock(bool block); // 阻塞和非阻塞属性设置
    uint32_t GetEpollEvents() const; // 返回已注册的EPOLL事件
    void SetEpollEvents(uint32_t events); // 设置已注册的EPOLL事件
private:
    int m_fd; // 句柄
    uint32_t m_events; // Epoll事件
    THandleType m_type; // 句柄类型，请参见THandleType一节
};

typedef enum
{
    htPipe, // 管道
    htTcpListener, // 监听者
    htTcpServant, // 服务端的侍者
    htTcpClient // 客户端
}THandleType;




#endif //PTF_5_CHANDLE_H
