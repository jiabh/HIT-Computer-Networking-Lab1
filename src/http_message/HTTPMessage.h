//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_HTTPMESSAGE_H
#define HTTP_PROXY_SERVER_HTTPMESSAGE_H

// 缓存大小 > 8KB，这样第一次读取一定能读完首部
#define BUFFER_LEN (1024 * 8 + 100)

#define VERSION_MAX_LEN 8

#include <map>
#include <string>

using std::map;
using std::string;

class HTTPMessage
{
public:
	char version[VERSION_MAX_LEN + 1]{'\0'}; // HTTP版本
	map<string, string> headers;
protected:
	int from_sock = -1; // 读取报文的套接字
	int to_sock = -1; // 发出报文的套接字

	char *buffer = nullptr;
	ssize_t bf_len = -1; // 当前的有效缓存大小

	char *body = nullptr; // 指向body的第一个字节
	size_t body_len = 0; // body长度

public:

	HTTPMessage(int from_sock, int to_sock);

	virtual ~HTTPMessage();

	/**
	 * 读取并解析报文
	 *
	 * @return 0 if success, -1 if error
	 */
	virtual int load() = 0;

	/**
	 * 发送报文
	 *
	 * @return 如果成功，返回发送的套接字；失败返回-1
	 */
	virtual int send() = 0;

protected:
	/**
	 * 读取报文中的headers和body
	 * 由于HTTP请求和相应在这部分的操作完全相同，故进行封装
	 * @param p_headers this->buffer中headers的第一个字节
	 * @return
	 */
	int load_headers_and_body(char *p_headers);
};


#endif //HTTP_PROXY_SERVER_HTTPMESSAGE_H
