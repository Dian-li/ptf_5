/*@file      connpool.h
* @brief     define connpool class, every PStep lease one from the pool to connect to the server;
* @author    liuxiaochuang
* @History
*    1.0.0: liuxiaochuang,  2017/09/14
*/

#ifndef _CONNPOOL_H_
#define _CONNPOOL_H_
#define CONNPOOLSIZE 1

#include <soci/soci.h>
#include <string>
#include<mutex>
#include <unordered_map>

using namespace soci;
using namespace std;


class TDBConnPool{
public:
    TDBConnPool(size_t pool_size, string conn_str);
    ~TDBConnPool();

    size_t lease();
    void give_back(size_t index);
    session & at(size_t index);

private:
    soci::connection_pool   m_conn_pool;
    size_t                  m_pool_size;
    string                  m_conn_str;
};

/*
class TDBConnMap{
public:
    static unordered_map<string, TDBConnPool*> * Instance(){
        return &m_conn_map;
    }
    static unordered_map<string, TDBConnPool*> m_conn_map;
private:
    TDBConnMap();
    TDBConnMap(TDBConnMap const &);
};*/

static unordered_map<string, TDBConnPool*>  conn_map;
static mutex                                db_map_mutex;




#endif
