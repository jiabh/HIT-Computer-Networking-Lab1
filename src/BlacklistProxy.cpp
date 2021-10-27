//
// Created by jiabh on 2021/10/27.
//

#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include "utils/socket_utl.h"
#include "logging.h"
#include "BlacklistProxy.h"

BlacklistProxy::BlacklistProxy(
		int client_sock,
		std::vector<string> &user_blacklist,
		std::vector<string> &host_blacklist) :
		Proxy(client_sock),
		user_blacklist(user_blacklist),
		host_blacklist(host_blacklist)
{}

void BlacklistProxy::run()
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

	err = forward_request();
	if (err == -1)
		return;

	err = load_response();
	if (err == -1)
		return;

	forward_response();
}

bool BlacklistProxy::black_user(char *user)
{
	return false;
}

bool BlacklistProxy::black_host(char *host)
{
	return false;
}
