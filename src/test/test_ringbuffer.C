/**
*
* @file      test_ringbuffer.C
* @brief     ringbuffer test
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "ringbuff.h"

typedef struct TQData
{
    pthread_t thread_id;
    TRingbuffer<int> *ring;
    int start;
    int total;
} TQData_t;

void* producer(void* arg)
{
    TQData_t* p = (TQData_t*)arg;
    if(NULL == p) return NULL;
    int n = p->start;
    for(int i=0; i<p->total; ++i)
    {
        int *pInt = new int(n++);
        bool ret = p->ring->enqueue(pInt);
        printf("Thread %d: >> %d  %s\n", p->thread_id, *pInt, (ret ? "" : "failed"));
        if(!ret)
        {
            delete pInt;
            pInt = NULL;
        }
    }
    printf("Thread %d exit\n", p->thread_id);
    return NULL;
}

void* consumer(void* arg)
{
    TQData_t* p = (TQData_t*)arg;
    if(NULL == p) return NULL;
    while(true)
    {
        int* pInt = p->ring->dequeue();
        if(NULL != pInt)
        {
            printf("Thread %d: << %d\n", p->thread_id, *pInt);
            delete pInt;
            pInt = NULL;
        }
        else
        {
            usleep(10);
        }
    }
    printf("Thread %d exit\n", p->thread_id);
    return NULL;
}

int main(int argc, char* argv[])
{
    int flags, opt;
    int nP=1, nC=1, nSize=100, total=100;
    TQData_t *pPInfo = NULL, *pCInfo = NULL;

    flags = 0;
    while ((opt = getopt(argc, argv, "hp:c:s:t:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            nP = atoi(optarg);
            if(nP<1) nP = 1;
            break;
        case 'c':
            nC = atoi(optarg);
            if(nC<1) nC = 1;
            break;
        case 's':
            nSize = atoi(optarg);
            if(nSize <= 1) nSize = 100;
            break;
        case 't':
            total = atoi(optarg);
            if(total < 1) total = 100;
            break;
        case 'h':
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-p | -c | -s | -t number]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
       }
    }
    printf("Producer number: %d\nConsumer number: %d\nRing size: %d\nTotal test: %d\n", nP, nC, nSize, total);
    TRingbuffer<int> ring(nSize, (nP>1 ? false : true), (nC > 1 ? false : true));
    pPInfo = new TQData_t[nP];
    pCInfo = new TQData_t[nC];
    for(int i=0; i<nP; ++i)
    {
        pPInfo[i].ring = &ring;
        pPInfo[i].start = i * total;
        pPInfo[i].total = total;
        int ret = pthread_create(&(pPInfo[i].thread_id), NULL, &producer, pPInfo+i);
        if(0 != ret)
        {
            printf("Create thread[%d] for producer failed, %s\n", strerror(ret));
        }
    }
    for(int i=0; i<nC; ++i)
    {
        pCInfo[i].ring = &ring;
        int ret = pthread_create(&(pCInfo[i].thread_id), NULL, &consumer, pCInfo+i);
        if(0 != ret)
        {
            printf("Create thread[%d] for consumer failed, %s\n", strerror(ret));
        }
    }

    for(int i=0; i<nP; ++i)
    {
        pthread_join(pPInfo[i].thread_id, NULL);
    }
    delete []pPInfo;
    for(int i=0; i<nC; ++i)
    {
        pthread_join(pCInfo[i].thread_id, NULL);
    }
    delete []pCInfo;
    return 0;
}

