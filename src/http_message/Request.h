//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_REQUEST_H
#define HTTP_PROXY_SERVER_REQUEST_H

#define URL_MAX_LEN 1024
#define METHOD_MAX_LEN 7

#include <map>
#include <stack>
#include <string>
#include "HTTPMessage.h"


class Request : public HTTPMessage
{

public:
	char method[METHOD_MAX_LEN + 1]{'\0'}; // 请求方法
	char url[URL_MAX_LEN + 1]{'\0'};

	std::string hostname;
	uint16_t port = 80;

	/**
	 * @param from_sock 读取报文的套接字。要求已经建立连接，等待read
	 * @param to_sock 可选，发出报文的套接字。如果设置，则要求已经建立连接，等待write
	 */
	explicit Request(int from_sock, int to_sock = -1);

	int load() override;

	int send() override;
};


#endif //HTTP_PROXY_SERVER_REQUEST_H
