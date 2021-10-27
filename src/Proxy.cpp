//
// Created by jiabh on 2021/10/26.
//


#include <iostream>
#include <cstring>
#include <unistd.h>
#include "http_message/Request.h"
#include "http_message/Response.h"
#include "utils/socket_utl.h"
#include "logging.h"
#include "Proxy.h"


Proxy::Proxy(
		int client_sock,
		std::vector<string> &user_blacklist,
		std::vector<string> &host_blacklist,
		std::map<string, string> &fishing_host) :

		client_sock(client_sock),
		target_sock(-1),
		client_request(client_sock),
		target_response(-1, -1), // invalid by default
		user_blacklist(user_blacklist),
		host_blacklist(host_blacklist),
		fishing_host(fishing_host)
{}

void Proxy::run()
{
	int err;
	err = load_request();
	if (err == -1)
		return;

	char client_ip[16];
	inet_ntoa_r(client_request.peer_addr.sin_addr, client_ip);

	for (auto &it: user_blacklist) // 过滤用户
		if (!strcmp(it.c_str(), client_ip))
		{
			log_warn("blocked a request from %s\n", client_ip);
			close(this->client_sock);
			return;
		}
	for (auto &it: host_blacklist) // 过滤网站
		if (it == client_request.hostname)
		{
			log_warn("blocked a request to %s\n", it.c_str());
			close(this->client_sock);
			return;
		}

	// 钓鱼
	if (!client_request.hostname.empty()) // 存在Host字段
	{
		auto fish_it = fishing_host.find(client_request.hostname);
		if (fish_it != fishing_host.end()) // 是目标网站，重写
		{
			client_request.hostname = fish_it->second;
			client_request.headers["Host"] = fish_it->second;
			log_info("Fish: %s -> %s\n", fish_it->first.c_str(), fish_it->second.c_str());
		}
	}

	err = forward_request();
	if (err == -1)
		return;

	err = load_response();
	if (err == -1)
		return;

	forward_response();
}

int Proxy::load_request()
{
	int err;

//	client_request已经在构造函数中初始化好，这里不需要初始化
//	client_request = Request(this->client_sock);

	// load request from client
	err = client_request.load();
	if (err == -1)
	{
		close(this->client_sock);
		return -1;
	}

	return 0;
}

int Proxy::forward_request()
{
	target_sock = client_request.send();
	if (target_sock == -1)
	{
		close(client_sock);
		return -1;
	}
	return 0;
}

int Proxy::load_response()
{
	int err;

	target_response = Response(target_sock, client_sock);

	// load response from server
	err = target_response.load();

	if (err == -1) // 关闭两个套接字
	{
		close(this->client_sock);
		close(this->target_sock);
		return -1;
	}
	return 0;
}

int Proxy::forward_response()
{
	int ret_sock = target_response.send();
	if (ret_sock == this->client_sock) // 成功发送，输出日志
	{
		char client_ip[16];
		inet_ntoa_r(client_request.peer_addr.sin_addr, client_ip);

		log_info("[%s:%hu] %s %s -> %s %s\n",
				 client_ip, client_request.peer_addr.sin_port,
				 client_request.method, client_request.url,
				 target_response.code, target_response.phrase
		);
	}

	close(this->client_sock);
	close(this->target_sock);
	return 0;
}
