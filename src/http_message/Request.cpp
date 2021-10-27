//
// Created by jiabh on 2021/10/26.
//

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../logging.h"
#include "../utils/str_utl.h"
#include "../utils/http_utl.h"
#include "../utils/socket_utl.h"
#include "HTTPMessage.h"
#include "Request.h"


Request::Request(int from_sock, int to_sock) : HTTPMessage(from_sock, to_sock)
{
}

int Request::load()
{
	buffer = new char[BUFFER_LEN]; // 申请缓存
	this->bf_len = 0;

	bf_len = read(this->from_sock, buffer, BUFFER_LEN);
	switch (this->bf_len)
	{
		case -1: // ERROR
			log_warn_with_msg("a socket read error occurred.");
			errno = 0;
			return -1;
		case 0: // EOF
			return -1;
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
		int err;

		// DNS解析
		hostent result_buf{}, *h_res;
		char char_buf[1024];
		int h_errnop;
		err = gethostbyname_r(this->hostname.c_str(),
							  &result_buf,
							  char_buf, sizeof(char_buf),
							  &h_res,
							  &h_errnop); // 可重入的DNS解析
		if (err || h_res == nullptr)
		{
			log_warn("could not found host %s.\n", this->hostname.c_str());
			return -1;
		}

		// 从此时，char_buf中存储的是host对应IP的字符串
		inet_ntoa_r(*((in_addr *) h_res->h_addr), char_buf);
//		log_info("host %s found, ip: %s\n", this->hostname.c_str(), char_buf); // host found log

		this->to_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 创建套接字
		if (to_sock < 0)
		{
			log_warn_with_msg("create socket failed.");
			errno = 0;
			return -1;
		}

		sockaddr_in target_addr = init_sockaddr_in(char_buf, this->port); // 与目标主机的套接字设置
		err = connect(to_sock, (sockaddr *) &target_addr, sizeof(target_addr));
		if (err < 0)
		{
			log_warn_with_msg("connect() to %s:%d failed.", this->hostname.c_str(), this->port);
			errno = 0;
			return -1;
		}
	}

	char sp[] = " ";
	char rn[] = "\r\n";

	// write request line
	const size_t Q_LEN = 6;
	char *sending_queue[Q_LEN] = {method, sp, url, sp, version, rn};
	for (auto s: sending_queue)
	{
		if (write(to_sock, s, strlen(s)) == -1) // write error
			return -1;
	}

	// write headers with body separator
	ssize_t ret;
	std::string h_str = headers2str(headers);
	h_str.append("\r\n");
	ret = write(to_sock, h_str.c_str(), h_str.length());
	if (ret == -1)
		return -1;

	// write body
	ret = write(to_sock, body, body_len);
	if (ret == -1)
		return -1;

	return this->to_sock;
}
