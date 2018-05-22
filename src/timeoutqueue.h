//
// Created by dian on 17-11-24.
//

#ifndef PTF_5_TIMEOUTQUEUE_H
#define PTF_5_TIMEOUTQUEUE_H

#include <queue>
#include <mutex>
#include "ctime"


using std::queue;
using std::mutex;

class TTransation;
struct TimeoutNode
{
    time_t m_time;
    long int m_name;
    TTransation* m_tTransation;
    TimeoutNode* m_next;
    TimeoutNode* m_parent;
};
class TimeoutQueue
{
public:
    TimeoutQueue();
    ~TimeoutQueue();
    void enTqueue(TimeoutNode* node);
    void deTqueue(TimeoutNode* node);
    void loop();


private:
    static void* lookuptime(void *arg);

public:
    int size;
    TimeoutNode* m_node;
};

static mutex m_tmutex;

#endif //PTF_5_TIMEOUTQUEUE_H
