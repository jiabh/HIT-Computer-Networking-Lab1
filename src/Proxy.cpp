//
// Created by jiabh on 2021/10/26.
//


#include <iostream>
#include <unistd.h>
#include "http_message/Request.h"
#include "http_message/Response.h"
#include "logging.h"
#include "Proxy.h"


Proxy::Proxy(int client_sock)
{
	this->client_sock = client_sock;
	this->target_sock = -1;
}

void Proxy::run()
{
	int err;
	int ret_sock;

	Request client_request(this->client_sock);

	// load request from client
	err = client_request.load();
	if (err == -1)
	{
		close(this->client_sock);
		return;
	}

	// send the request
	this->target_sock = client_request.send();
	if (this->target_sock == -1)
	{
		close(this->client_sock);
		return;
	}

	Response target_response = Response(this->target_sock, this->client_sock);

	// load response from server
	err = target_response.load();
	if (err == -1) // 关闭两个套接字
		goto quit;

	// send the response back to the client
	ret_sock = target_response.send();
	if (ret_sock == this->client_sock) // 成功发送，输出日志
	{
		log_info("%s %s -> %s %s\n",
				 client_request.method, client_request.url,
				 target_response.code, target_response.phrase
		);
	}

	quit:
	close(this->client_sock);
	close(this->target_sock);
}
