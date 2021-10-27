//
// Created by jiabh on 2021/10/26.
//


#include <iostream>
#include <unistd.h>
#include "http_message/Request.h"
#include "http_message/Response.h"
#include "utils/socket_utl.h"
#include "logging.h"
#include "Proxy.h"


Proxy::Proxy(int client_sock) : client_sock(client_sock),
								target_sock(-1),
								client_request(client_sock),
								target_response(-1, -1) // invalid by default
{}

void Proxy::run()
{
	int err;
	err = load_request();
	if (err == -1)
		return;

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

	quit:
	close(this->client_sock);
	close(this->target_sock);
	return 0;
}
