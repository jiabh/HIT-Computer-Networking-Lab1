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
	/**
	 * 进行一次read，缓存在this->buffer中，解析其中的响应头部分
	 */

	buffer = new char[BUFFER_LEN]; // 申请缓存
	this->bf_len = 0;

	bf_len = read(this->from_sock, buffer, BUFFER_LEN);
	switch (this->bf_len)
	{
		case -1: // ERROR
			log_error_with_msg("a socket read error occurred.");
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
	std::pair<size_t, std::map<std::string, std::string>> headers_res = parse_headers(p_headers); // 要算上请求行末尾的\r\n
	size_t headers_len = headers_res.first;
	this->headers = headers_res.second;

	/* 设置并读取body */
	p_buf = p_headers + headers_len + 2;
	auto te_item = headers.find("Transfer-Encoding");
	auto cl_item = headers.find("Content-Length");
	if (te_item != headers.end() && te_item->second == "chunked")
		body_type = BodyType::Chunked;
	else if (cl_item != headers.end())
		body_type = BodyType::ContentLength;
	else
		body_type = BodyType::Unknown;

	if (body_type == BodyType::Chunked)
	{
		/* 当设置Transfer-Encoding: chunked时，不在这里解析body，在send解析 */
		// 将buffer中剩余的chunk主体移到buffer的前面
		size_t left_buf = bf_len - (size_t) (p_buf - buffer); // buffer中剩余有效缓存的大小
		memmove(buffer, p_buf, left_buf);
		bf_len = (ssize_t) left_buf; // 更新有效缓存长度
	} else
	{
		if (body_type == BodyType::ContentLength)
			this->body_len = std::stoi(cl_item->second);
		else
			this->body_len = (ssize_t) (buffer + bf_len - p_buf);

		// 申请body并写入
		// 从此时起，bf_len指代缓冲区中body的字节数
		this->body = new char[this->body_len];
		bf_len -= (ssize_t) (p_buf - buffer);
		size_t body_left = body_len;
		memcpy(this->body, p_buf, bf_len); // 将当前缓冲区中的body拷贝过去
		body_left -= bf_len;
		while (body_left) // 如果存在，获取剩余的body
		{
			bf_len = read(this->from_sock, buffer, BUFFER_LEN);
			switch (this->bf_len)
			{
				case -1: // ERROR
					log_error("a socket read error occurred.\n");
					errno = 0;
					return -1;
				case 0: // EOF
					return -1;
				default: // correct
					memcpy(this->body + body_len - body_left, buffer, bf_len); // 将当前缓冲区中的body拷贝过去
					body_left -= bf_len;
					break;
			}
		}

		/* 读取完毕，释放缓冲区 */
		delete[] buffer;
		buffer = nullptr;
	}

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

	if (body_type == BodyType::Chunked)
	{
		// 此时buffer未释放，buffer[0]存放着body部分的第一个字节，以此类推
		ssize_t chunk_size;
		while (true) // 每轮循环发走一个chunk
		{
			chunk_size = get_chunk_size(buffer); // 该块在this->buffer中的剩余大小
			if (chunk_size < 0) // 块大小解析失败
				return -1;
			else if (chunk_size == 0) // 最后一块
			{
				ret = write(to_sock, "0\r\n\r\n", 5);
				if (ret == -1)
					return -1;
			}

			chunk_size += KMP(buffer, bf_len, "\r\n", 2) + 2; // 把块大小后面的\r\n算上
			while (chunk_size > 0) // "发送缓存-收取"循环，直至发完该块
			{
				if (chunk_size >= bf_len) // 块未被接收完
				{
					// 发走收取的全部chunk
					ret = write(to_sock, buffer, bf_len);
					if (ret == -1)
						return -1;
					chunk_size -= bf_len;

					// 继续读
					bf_len = read(this->from_sock, buffer, BUFFER_LEN);
					switch (this->bf_len)
					{
						case -1: // ERROR
							log_error("a socket read error occurred.\n");
							errno = 0;
							return -1;
						case 0: // EOF
							return -1;
						default: // correct
							break;
					}
				} else // 块全部被缓存
				{
					// 发走该块的剩余部分
					ret = write(to_sock, buffer, chunk_size);
					if (ret == -1)
						return -1;
					ret = write(to_sock, "\r\n", 2);
					if (ret == -1)
						return -1;

					// 把buffer中剩下的数据移动到开头来
					size_t left_buf = bf_len - chunk_size - 2; // buffer中剩余有效缓存的大小，-2是为了跳过块末的\r\n
					if (left_buf > 0) // 有有效数据
					{
						memmove(buffer, buffer + bf_len - left_buf, left_buf);
						bf_len = (ssize_t) left_buf;
					} else // 没有有效数据，read一次
					{
						bf_len = read(this->from_sock, buffer, BUFFER_LEN);
						switch (this->bf_len)
						{
							case -1: // ERROR
								log_error("a socket read error occurred.\n");
								errno = 0;
								return -1;
							case 0: // EOF
								return -1;
							default: // correct
								break;
						}
					}
					break;
				}
			}
		}
	} else
	{
		// write body
		ret = write(to_sock, body, body_len);
		if (ret == -1)
			return -1;
	}
	return to_sock;
}
