//
// Created by jiabh on 2021/10/27.
//

#ifndef HTTP_PROXY_SERVER_BLACKLISTPROXY_H
#define HTTP_PROXY_SERVER_BLACKLISTPROXY_H

#include <vector>
#include <string>
#include "Proxy.h"

class BlacklistProxy : public Proxy
{
public:
	BlacklistProxy(
			int client_sock,
			std::vector<string> &user_blacklist,
			std::vector<string> &host_blacklist
	);

	void run() override;

protected:
	std::vector<std::string> user_blacklist;
	std::vector<std::string> host_blacklist;

	bool black_user(char *user);

	bool black_host(char *host);

};


#endif //HTTP_PROXY_SERVER_BLACKLISTPROXY_H
