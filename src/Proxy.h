//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_PROXY_H
#define HTTP_PROXY_SERVER_PROXY_H

#include <vector>
#include <string>
#include "http_message/Request.h"
#include "http_message/Response.h"

class Proxy
{
protected:
	int client_sock; // 与客户端间的套接字
	int target_sock; // 与目标服务器间的套接字

	Request client_request;
	Response target_response;

	std::vector<std::string> user_blacklist;
	std::vector<std::string> host_blacklist;

public:
	/**
	 * 已经accept客户端连接的套接字
	 *
	 * @param client_sock 与客户端连接的套接字
	 * @param user_blacklist 用户黑名单
	 * @param host_blacklist 主机黑名单
	 */
	Proxy(
			int client_sock,
			std::vector<string> &user_blacklist,
			std::vector<string> &host_blacklist
	);

	/**
	 * 开始代理
	 */
	virtual void run();

protected:
	int load_request();

	int forward_request();

	int load_response();

	int forward_response();
};


#endif //HTTP_PROXY_SERVER_PROXY_H
