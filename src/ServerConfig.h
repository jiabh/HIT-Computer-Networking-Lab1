//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_SERVERCONFIG_H
#define HTTP_PROXY_SERVER_SERVERCONFIG_H

class ServerConfig
{
public:
	char address[16] = "127.0.0.1"; // 监听地址
	uint16_t port = 2021;           // 监听端口
	size_t th_cnt = 20;             // 线程个数

	/**
	 * 从命令行中读取配置，如果有误直接退出程序
	 * @param argc
	 * @param argv
	 * @return 读取后的ServerConfig
	 */
	static ServerConfig from_arg(int argc, char *argv[]);

	friend std::ostream &operator<<(std::ostream &out, ServerConfig &A);
};

#endif //HTTP_PROXY_SERVER_SERVERCONFIG_H
