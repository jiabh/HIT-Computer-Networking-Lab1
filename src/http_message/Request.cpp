//
// Created by jiabh on 2021/10/26.
//

// 缓存大小 > 8KB，这样第一次读取一定能读完首部
#define BUFFER_LEN (1024 * 8 + 100)

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../logging.h"
#include "../util.h"
#include "util.h"
#include "HTTPMessage.h"
#include "Request.h"


Request::Request(int from_sock, int to_sock)
{
	this->from_sock = from_sock;
	this->to_sock = to_sock;
}

int Request::load()
{
	buffer = new char[BUFFER_LEN]; // 申请缓存
	this->bf_len = 0;

	bf_len = read(this->from_sock, buffer, BUFFER_LEN);
	switch (this->bf_len)
	{
		case -1: // ERROR
			log_warn("a socket read error occurred.\n");
			return 1;
		case 0: // EOF
			return 1;
		default: // correct
			break;
	}

	/* 解析请求行 */
	std::pair<size_t, std::vector<size_t>> res = split_str(buffer, " ", "\r\n");
	size_t src_data_len = res.first;
	std::vector<size_t> request_line_index = res.second;

	if (request_line_index.size() != 3) // 非法HTTP字符串
	{
		log_warn("get an invalid HTTP client request.\n");
		return -1;
	}

	size_t deli_len = 1; // 分隔符长度，空格做分隔符，故为1
	size_t m_i = request_line_index[0]; // method索引
	size_t url_i = request_line_index[1]; // URL索引
	size_t v_i = request_line_index[2]; // HTTP版本索引
	// 防止下标溢出，取输入数据和最大长度中的最小值
	const size_t m_cpy_len = std::min(url_i - m_i - deli_len, (size_t) METHOD_MAX_LEN);
	const size_t url_cpy_len = std::min(v_i - url_i - deli_len, (size_t) URL_MAX_LEN);
	const size_t v_cpy_len = std::min(src_data_len - v_i + 1, (size_t) VERSION_MAX_LEN);

	memcpy(method, buffer + m_i, m_cpy_len);
	method[m_cpy_len] = '\0';
	memcpy(url, buffer + url_i, url_cpy_len);
	url[url_cpy_len] = '\0';
	memcpy(version, buffer + v_i, v_cpy_len);
	version[v_cpy_len] = '\0';

	/* 解析首部行 */
	char *p_headers = buffer + src_data_len + 2;
	this->load_headers_and_body(p_headers);

	/* 读取完毕，释放缓冲区 */
	delete[] buffer;
	buffer = nullptr;

	// 设置host
	std::string &host = this->headers["Host"];
	size_t colon_i = host.find(':');
	if (colon_i == -1)
	{
		this->hostname = host;
		this->port = 80;
	} else
	{
		this->hostname = host.substr(0, colon_i);
		this->port = std::stoi(host.substr(colon_i + 1));
	}

	return 0;
}

int Request::send()
{
	if (this->to_sock < 0) // 未手动设置发出报文的套接字，进行连接
	{
		// DNS解析
		hostent *h = gethostbyname(this->hostname.c_str());
		if (h == nullptr)
		{
			log_warn("could not found host %s.", this->hostname.c_str());
			return -1;
		}
		log_success("host %s found, ip: %s\n", this->hostname.c_str(), inet_ntoa(*((in_addr *) h->h_addr)));

		this->to_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 创建套接字
		if (to_sock < 0)
		{
			log_error("create socket failed (return %d)\n", to_sock);
			return -1;
		}

		sockaddr_in target_addr = init_sockaddr_in( // 与目标主机的套接字设置
				inet_ntoa(*((in_addr *) h->h_addr)),
				this->port
		);
		int err = connect(to_sock, (sockaddr *) &target_addr, sizeof(target_addr));
		if (err < 0)
		{
			log_warn("connect() to %s:%d failed (return %d)\n",
					 this->hostname.c_str(), this->port, err);
			return -1;
		}
	}

	// write client_request line
	write(to_sock, method, strlen(method));
	write(to_sock, " ", 1);
	write(to_sock, url, strlen(url));
	write(to_sock, " ", 1);
	write(to_sock, version, strlen(version));
	write(to_sock, "\r\n", 2);

	// write headers with body separator
	std::string h_str = headers2str(headers);
	h_str.append("\r\n");
	write(to_sock, h_str.c_str(), h_str.length());

	// write body
	write(to_sock, body, body_len);

	return this->to_sock;
}
