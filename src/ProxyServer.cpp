//
// Created by jiabh on 2021/10/26.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include "logging.h"
#include "http_message/Request.h"
#include "http_message/util.h"
#include "Proxy.h"
#include "ProxyServer.h"


ProxyServer::ProxyServer(ServerConfig config)
{
	this->config = config;
}

int ProxyServer::init()
{
	int err;

	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 创建套接字
	if (serv_sock < 0)
	{
		log_error("create socket failed (return %d)\n", this->serv_sock);
		exit(1);
	}

	sockaddr_in serv_addr = init_sockaddr_in(config.address, config.port);
	err = bind(serv_sock, (sockaddr *) &serv_addr, sizeof(serv_addr));
	if (err < 0)
	{
		log_error("bind() to %s:%d failed (return %d)\n",
				  this->config.address, this->config.port, this->serv_sock);
		exit(1);
	}

	listen(serv_sock, 20);
	log_success("listening %s:%d\n", this->config.address, this->config.port);

	return 0;
}

[[noreturn]] void ProxyServer::start() const
{
	sockaddr_in client_addr{};
	socklen_t client_addr_size = sizeof(client_addr);
	int client_sock, proxy_sock;

	while (true)
	{
		client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size);
		if (client_sock == -1)
		{
			log_warn("An accept error occurred.\n");
			continue;
		}

		Proxy proxy(client_sock);
		proxy.run();


	}
}
