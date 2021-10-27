//
// Created by jiabh on 2021/10/26.
//

#include <cstring>
#include <unistd.h>
#include <map>
#include <string>
#include "util.h"
#include "../logging.h"
#include "HTTPMessage.h"

int HTTPMessage::load_headers_and_body(char *p_headers)
{
	std::pair<size_t, std::map<std::string, std::string>> headers_res = parse_headers(p_headers); // 要算上请求行末尾的\r\n
	size_t headers_len = headers_res.first;
	this->headers = headers_res.second;

	/* 设置并读取body */
	char *p_body = p_headers + headers_len + 2;
	auto cl_item = headers.find("Content-Length");
	if (cl_item != headers.end()) // 存在Content-Length字段
		this->body_len = std::stoi(cl_item->second);
	else // 没有设置body大小，设置成当前buffer的剩余部分
		this->body_len = (ssize_t) (buffer + bf_len - p_body);

	// 申请body并写入
	// 从此时起，bf_len指代缓冲区中body的字节数
	this->body = new char[this->body_len];
	bf_len -= (ssize_t) (p_body - buffer);
	size_t body_left = body_len;
	memcpy(this->body, p_body, bf_len); // 将当前缓冲区中的body拷贝过去
	body_left -= bf_len;
	while (body_left) // 如果存在，获取剩余的body
	{
		bf_len = read(this->from_sock, buffer, BUFFER_LEN);
		switch (this->bf_len)
		{
			case -1: // ERROR
				log_warn("a socket read error occurred.\n");
				return 1;
			case 0: // EOF
				return 1;
			default: // correct
				memcpy(this->body + body_len - body_left, buffer, bf_len); // 将当前缓冲区中的body拷贝过去
				body_left -= bf_len;
				break;
		}
	}
	return 0;
}
