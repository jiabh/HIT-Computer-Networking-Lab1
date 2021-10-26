//
// Created by jiabh on 2021/10/26.
//

#include <map>
#include <string>
#include "../util.h"
#include "util.h"


sockaddr_in init_sockaddr_in(char *address, uint16_t port)
{
	sockaddr_in addr{};
	addr.sin_family = AF_INET; // IPv4
	addr.sin_addr.s_addr = inet_addr(address); // 设置地址
	addr.sin_port = htons(port); // 设置端口
	return addr;
}

std::pair<size_t, std::map<std::string, std::string>> parse_headers(char *message)
{
	std::map<std::string, std::string> headers;

	char *p_start = message;
	char *p_end = message;
	while (p_start[0] != '\r' && p_start[1] != '\n' && p_start[2] != '\r' && p_start[3] != '\n')
	{
		std::string key, value;

		while (*p_end != ':')
			p_end++;
		key.assign(p_start, (size_t) (p_end - p_start));

		p_end++;
		p_start = p_end;
		while (*p_end != '\r' && *(p_end + 1) != '\n')
			p_end++;
		value.assign(p_start, (size_t) (p_end - p_start));
		p_start = p_end + 2;

		trim(key);
		trim(value);
		headers[key] = value;
	}

	auto headers_len = (size_t) (p_start - message);
	return std::make_pair(headers_len, headers);
}

std::string headers2str(std::map<std::string, std::string> &headers)
{
	std::string res;

	std::map<std::string, std::string>::iterator it;;
	for (it = headers.begin(); it != headers.end(); it++)
	{
		res.append(it->first).append(": ").append(it->second).append("\r\n");
	}

	return res;
}
