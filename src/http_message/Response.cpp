//
// Created by jiabh on 2021/10/26.
//

#define BUFFER_LEN (1024 * 8 + 100)

#include <cstring>
#include <unistd.h>
#include <vector>
#include "../logging.h"
#include "../utils/str_utl.h"
#include "../utils/http_utl.h"
#include "Response.h"

Response::Response(int from_sock, int to_sock) : HTTPMessage(from_sock, to_sock)
{
}

int Response::load()
{
	buffer = new char[BUFFER_LEN]; // 申请缓存
	this->bf_len = 0;

	bf_len = read(this->from_sock, buffer, BUFFER_LEN);
	switch (this->bf_len)
	{
		case -1: // ERROR
			log_error_with_msg("a socket read error occurred.");
			errno = 0;
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

	if (request_line_index.size() < 3) // 非法HTTP字符串
	{
		log_warn("get an invalid HTTP client_request.\n");
		return -1;
	}

	size_t deli_len = 1; // 分隔符长度，空格做分隔符，故为1
	size_t v_i = request_line_index[0]; // HTTP版本索引
	size_t code_i = request_line_index[1]; // 状态码索引
	size_t phrase_i = request_line_index[2]; // 状态短语索引
	// 防止下标溢出，取输入数据和最大长度中的最小值
	const size_t v_cpy_len = std::min(code_i - v_i - deli_len, (size_t) VERSION_MAX_LEN);
	const size_t c_cpy_len = std::min(phrase_i - code_i - deli_len, (size_t) CODE_MAX_LEN);
	const size_t p_cpy_len = std::min(src_data_len - phrase_i + 1, (size_t) PHRASE_MAX_LEN);

	memcpy(version, buffer + v_i, v_cpy_len);
	version[v_cpy_len] = '\0';
	memcpy(code, buffer + code_i, c_cpy_len);
	code[c_cpy_len] = '\0';
	memcpy(phrase, buffer + phrase_i, p_cpy_len);
	phrase[p_cpy_len] = '\0';

	/* 解析首部行 */
	char *p_headers = buffer + src_data_len + 2;
	this->load_headers_and_body(p_headers);

	/* 读取完毕，释放缓冲区 */
	delete[] buffer;
	buffer = nullptr;

	return 0;
}

int Response::send()
{
	char sp[] = " ";
	char rn[] = "\r\n";

	// write status line
	const size_t Q_LEN = 6;
	char *sending_queue[Q_LEN] = {version, sp, code, sp, phrase, rn};
	for (auto s: sending_queue)
	{
		if (write(to_sock, s, strlen(s)) == -1)
			return -1;
	}

	// write headers with body separator
	ssize_t ret;
	std::string h_str = headers2str(headers);
	h_str.append(rn);
	ret = write(to_sock, h_str.c_str(), h_str.length());
	if (ret == -1)
		return -1;

	// write body
	ret = write(to_sock, body, body_len);
	if (ret == -1)
		return -1;

	return to_sock;
}
