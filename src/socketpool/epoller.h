//
// Created by dian.
//

#ifndef PTF_5_EPOLLER_H
#define PTF_5_EPOLLER_H

class CEpoller
{
public:
    bool create(uint32_t size); // 创建一个epoll，其中size是epoll大小
    void destroy(); // 销毁epoll
    bool AddEvents(CHandle* pHandle, uint32_t events); // 添加一个CHandle
    bool ModEvents(CHandle* pHandle, uint32_t events); // 修改一个CHandle
    bool DelEvents(CHandle* pHandle); // 删除一个CHandle
    int TimedWait(uint32_t milliSeconds); // 毫秒级超时等待
    CHandle* GetHandle(int index) const; // TimedWait成功返回后，通过这个方法得到是哪个CHandle
};

#endif //PTF_5_EPOLLER_H
