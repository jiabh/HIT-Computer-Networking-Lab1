//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_PROXYSERVER_H
#define HTTP_PROXY_SERVER_PROXYSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ServerConfig.h"

class ProxyServer
{
private:
	int serv_sock = 0;
public:
	ServerConfig config;

	explicit ProxyServer(ServerConfig config);

	/**
	 * 初始化代理服务器，如果出错则直接退出程序
	 * @return
	 */
	int init();

	/**
	 * 启动服务器
	 */
	[[noreturn]] void start() const;
};


#endif //HTTP_PROXY_SERVER_PROXYSERVER_H
