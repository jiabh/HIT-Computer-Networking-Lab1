//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_RESPONSE_H
#define HTTP_PROXY_SERVER_RESPONSE_H

#define CODE_MAX_LEN 3
#define PHRASE_MAX_LEN 20

#include "HTTPMessage.h"

class Response : public HTTPMessage
{
public:
	enum class BodyType
	{
		Chunked,       // Transfer-Encoding: chunked
		ContentLength, // Content-Length: XXX
		Unknown        // 未知传输格式
	};
	BodyType body_type = BodyType::Unknown;
protected:
	char *p_buf = buffer; // 指向当前buffer中的第一个有效位置
public:
	char code[CODE_MAX_LEN + 1]{'\0'}; // 状态码
	char phrase[PHRASE_MAX_LEN + 1]{'\0'}; // 状态短语

	/**
	 * @param from_sock 读取报文的套接字。要求已经建立连接，等待read
	 * @param to_sock 发出报文的套接字。要求已经建立连接，等待write
	 */
	explicit Response(int from_sock, int to_sock);

	int load() override;

	int send() override;
};


#endif //HTTP_PROXY_SERVER_RESPONSE_H
