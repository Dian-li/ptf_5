#include "connpool.h"
#include <stdio.h>


TDBConnPool::TDBConnPool(size_t pool_size, string conn_str): m_conn_pool(pool_size), m_pool_size(pool_size), m_conn_str(conn_str){
    printf("====%s ready to create %d connection=====\n", conn_str.c_str(), pool_size);
    for (int i=0; i<m_pool_size; i++) {
        session &sql = m_conn_pool.at(i);
        try {
            sql.open(m_conn_str);
        } catch (exception const & e) {
            printf("in TDBConnPool's ctor::error:%s\n", e.what());
            i--;
        }

    }
}

TDBConnPool::~TDBConnPool(){
        for (int i=0; i<m_pool_size; i++) {
            session &sql = m_conn_pool.at(i);
            sql.close();
        }
    }

size_t TDBConnPool::lease() {
	return m_conn_pool.lease();
}

void TDBConnPool::give_back(size_t index) {
	m_conn_pool.give_back(index);
}

session & TDBConnPool::at(size_t index) {
    return m_conn_pool.at(index);
}
//unordered_map<string, TDBConnPool*> TDBConnMap::m_conn_map;
