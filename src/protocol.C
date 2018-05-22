/*
 * protocol.C
 *
 *      Author: dian
 */
#include <stdlib.h>
#include <iostream>
#include "protocol.h"

TAppProtocolUD::TAppProtocolUD(const char* file) {
//	const char* pFile = file;
//	char filePath[512];
//	if (likely('/' != *file)) {
//		sprintf(filePath, "%s/etc/%s", getenv("PTF_HOME"), file);
//		pFile = filePath;
//	}
//	config_t* conf = new config_t;
//	config_init(conf);
//	int ret = config_read_file(conf, pFile);
//	if (unlikely(0 == ret)) {
//		printf("load config %s failed, configuration not changed.\n", pFile);
//		return false;
//	}
//	if (NULL != m_conf) {
//		config_destroy (m_conf);
//	}
}
TAppProtocolTable::TAppProtocolTable() {
	m_protocols = NULL;
	m_content_len = -1;
	m_chunkOrNot = false;
	m_headerLen = 0;
	m_findContentLength = false;
	m_finish = false;
}

TAppProtocolTable::~TAppProtocolTable() {
}

int TAppProtocolTable::receiveHttpResponse(evbuffer * buffer) {
	if (buffer == NULL) {
		//printf("buffer is null\n");
		return -1;
	}
	int buffer_length = evbuffer_get_length(buffer);
	 //printf("buffer 长度 %d\n",buffer_length);
	int header_mark= evbuffer_search(buffer, "\x0d\x0a\x0d\x0a", 4,
	 NULL).pos;//查询http中header与body之间的标志
	if(header_mark<0){
		//printf("未找到header mark!\n");
		return -1;
	}else{//头部齐全
		 struct evbuffer_ptr  not_chunk_mark = evbuffer_search(buffer,"Content-Length",14,NULL);
		 struct evbuffer_ptr chunk_mark = evbuffer_search(buffer,"chunked",7,NULL);
		 if(not_chunk_mark.pos>0){
			 //printf("找到not chunked mark\n");
			 int content_length_tail = evbuffer_search(buffer,"\x0d\x0a",2,&not_chunk_mark).pos;
			 if(content_length_tail>0){
				 int seg_len = content_length_tail - not_chunk_mark.pos - 4;
				 //printf("seg_len is %d\n",seg_len);
				 return 1;
			 }else{
				 return -1;
			 }

		 }else if(chunk_mark.pos >0){
			 int len = evbuffer_search(buffer, "\x0d\x0a\x30\x0d\x0a\x0d\x0a", 7, NULL).pos;
			 if(len<0){
				 return -1;
			 }else{
				 int buffer_length1 = evbuffer_get_length(buffer);
				 //printf("chunked is OK,buffer length is %d\n",buffer_length1);
				 //printf("chunked is OK,length is %d\n",len);
				 return len + 7;
			 }
		 }else{//不齐全
			 //printf("未找到body结尾\n");
			 return -1;
		 }

	}
}

