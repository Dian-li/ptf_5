/*
 * protocol.h
 *
 *      Author: dian
 */

#ifndef SRC_PROTOCOL_H_
#define SRC_PROTOCOL_H_

#include <libconfig.h>
#include <cstring>
#include <event2/buffer.h>

class TAppProtocolUD {
private:
#  define MaxProtocolNameLength 32
	char name[MaxProtocolNameLength]; //协议名称

# define st_fixed_length 0　//固定消息长度
# define st_separator 1　　//切割符
# define st_head_body 2　　//切割符分割消息头与消息体
	int separateType; //切割类型

#  define MaxSeparateStrLength 32
	char separateStr[MaxSeparateStrLength]; //切割字符,separator模式为切割符，head_body模式为切割head、body的分隔符

	int separateStrLen; //切割字符长度；
# define sm_null -1
#  define sm_start 0
#  define sm_end   1
	int separateMode; //切割模式

	int lengthLength; //消息长度域的长度
#  define length_type_binary 0
#  define length_type_ascii  1
	int lengthType; //长度类型

	int tailLength; //消息结尾域的长度

#  define MaxSeparateStrLength 32
	char skipStr[MaxSeparateStrLength]; //不算做统计长度的字符串

	int skipStrLen; //不算做统计长度的长度

public:
	TAppProtocolUD(const char* file);
	~TAppProtocolUD();

};

class TAppProtocolTable {

private:
	TAppProtocolUD* m_protocols;
	int m_content_len;
	bool m_chunkOrNot;
	int m_headerLen;
	bool m_findContentLength;
	bool m_finish;

public:
	TAppProtocolTable();
	~TAppProtocolTable();
	void receive(const char* protocol, evbuffer * buffer);
	int receiveHttpResponse(evbuffer * buffer);

};

#endif /* SRC_PROTOCOL_H_ */
