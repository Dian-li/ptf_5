#ifndef _REDIS_H_
#define	_REDIS_H_

#include<iostream>
#include<string.h>
#include<string>
#include<stdio.h>
#include <mutex>

#include<hiredis/hiredis.h>

class Redis{
public:
	Redis(){}
	~Redis(){
		this->m_connect = NULL;
		this->m_reply = NULL;
	}
	bool connect(std::string host, int port){
		m_connect = redisConnect(host.c_str(), port);
		if(m_connect != NULL && m_connect->err) {
			printf("connect error: %s\n", m_connect->errstr);
			return false;
		}
		return true;
	}
	
	std::string get(std::string key) {
		m_reply = (redisReply*)redisCommand(m_connect, "GET %s", key.c_str());
		std::string str = m_reply->str;
		freeReplyObject(m_reply);
		return str;
	}

	void set(std::string key, std::string value) {
		redisCommand(m_connect, "SET %s %s", key.c_str(), value.c_str());
	}

	void free(){
		redisFree(m_connect);
	}

	std::mutex m_redis_mutex;
private:
	redisContext* m_connect;
	redisReply* m_reply;
};

#endif // _REDIS_H_
