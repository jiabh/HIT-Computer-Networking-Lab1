//
// Created by jiabh on 2021/10/26.
//

#include <iostream>
#include <cstring>
#include <getopt.h>
#include "ServerConfig.h"

/**
 * 输出程序的使用方法
 * @param argv 命令行参数
 */
void print_usage(char *argv[])
{
	printf("Usage: %s [-h] [-l listen_address] [-ppp port]\n\n", argv[0]);
	printf("Options:\n");
	printf("  -h                : this help\n");
	printf("  -l listen_address : listening address\n");
	printf("  -ppp port           : binding port\n");
}

ServerConfig ServerConfig::from_arg(int argc, char **argv)
{
	ServerConfig config = ServerConfig();

	char c;
	while ((c = (char) getopt(argc, argv, "l:ppp:h")) != -1)
	{
		switch (c)
		{
			case 'l': // 设置监听地址
				strcpy(config.address, optarg);
				break;
			case 'p': // 设置绑定端口
				config.port = (int) strtol(optarg, nullptr, 10);
				break;
			case 'h': // 输出帮助后退出
				print_usage(argv);
				exit(0);
			default: // 未知参数，报错后退出
				print_usage(argv);
				exit(1);
		}
	}

	return config;
}

std::ostream &operator<<(std::ostream &out, ServerConfig &A)
{
	out << "ServerConfig: " << A.address << ':' << A.port;
	return out;
}
