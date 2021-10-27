//
// Created by jiabh on 2021/10/26.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <vector>
#include <string>
#include "ThreadPool.h"
#include "logging.h"
#include "http_message/Request.h"
#include "utils/http_utl.h"
#include "set_signals.h"
#include "Proxy.h"
#include "ProxyServer.h"


ProxyServer::ProxyServer(ServerConfig config)
{
	this->config = config;

	clear_sig_pipe(); // 屏蔽SIGPIPE信号
}

int ProxyServer::init()
{
	/**
	 * 参考链接：一个简单的Linux下的socket程序
	 * http://c.biancheng.net/cpp/html/3030.html
	 */
	int err;

	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 创建套接字
	if (serv_sock < 0)
	{
		log_error_with_msg("create socket failed.");
		exit(1);
	}

	sockaddr_in serv_addr = init_sockaddr_in(config.address, config.port);
	err = bind(serv_sock, (sockaddr *) &serv_addr, sizeof(serv_addr));
	if (err < 0)
	{
		log_error_with_msg("bind() to %s:%d failed.", this->config.address, this->config.port);
		exit(1);
	}

	listen(serv_sock, 20);
	log_success("listening %s:%d\n", this->config.address, this->config.port);

	return 0;
}

[[noreturn]] void ProxyServer::start() const
{
	std::vector<string> user_blacklist; // 用户IP黑名单
	std::vector<string> host_blacklist; // 域名IP黑名单

//	user_blacklist = {"127.0.0.1"};
//	host_blacklist = {"unlock-music.jiabh.cn"};

	// 线程池
	ThreadPool pool(this->config.th_cnt);

	// 客户端套接字配置
	sockaddr_in client_addr{};
	socklen_t client_addr_size = sizeof(client_addr);

	pool.start();
	while (true)
	{
		int client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size);
		if (client_sock == -1)
		{
			log_error("an accept error occurred.\n");
			errno = 0;
			continue;
		}

		auto *p_proxy = new Proxy(client_sock, user_blacklist, host_blacklist);
		pool.add_task(
				[ObjectPtr = p_proxy]
				{
					ObjectPtr->run(); // 运行该代理
					delete ObjectPtr; // 释放资源
				}
		);
	}
}
