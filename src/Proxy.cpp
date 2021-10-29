//
// Created by jiabh on 2021/10/26.
//


#include <iostream>
#include <cstring>
#include <unistd.h>
#include "http_message/Request.h"
#include "http_message/Response.h"
#include "utils/socket_utl.h"
#include "utils/time_utl.h"
#include "utils/str_utl.h"
#include "logging.h"
#include "cache/Cache.h"
#include "Proxy.h"


Proxy::Proxy(
		int client_sock,
		std::vector<string> &user_blacklist,
		std::vector<string> &host_blacklist,
		std::map<string, string> &fishing_host) :

		client_sock(client_sock),
		target_sock(-1),
		client_request(client_sock),
		target_response(-1, -1), // invalid by default
		user_blacklist(user_blacklist),
		host_blacklist(host_blacklist),
		fishing_host(fishing_host)
{}

void Proxy::run()
{
	int err;
	err = load_request();
	if (err == -1)
		return;

	char client_ip[16];
	inet_ntoa_r(client_request.peer_addr.sin_addr, client_ip);

	for (auto &it: user_blacklist) // 过滤用户
		if (!strcmp(it.c_str(), client_ip))
		{
			log_warn("blocked a request from %s\n", client_ip);
			close(this->client_sock);
			return;
		}
	for (auto &it: host_blacklist) // 过滤网站
		if (it == client_request.hostname)
		{
			log_warn("blocked a request to %s\n", it.c_str());
			close(this->client_sock);
			return;
		}

	// 钓鱼
	if (!client_request.hostname.empty()) // 存在Host字段
	{
		auto fish_it = fishing_host.find(client_request.hostname);
		if (fish_it != fishing_host.end()) // 是目标网站，重写
		{
			client_request.hostname = fish_it->second;
			client_request.headers["Host"] = fish_it->second;
			log_info("Fish: %s -> %s\n", fish_it->first.c_str(), fish_it->second.c_str());
		}
	}

	err = forward_request();
	if (err == -1)
		return;

	err = load_response();
	if (err == -1)
		return;

	forward_response();
}

int Proxy::load_request()
{
	int err;

//	client_request已经在构造函数中初始化好，这里不需要初始化
//	client_request = Request(this->client_sock);

	// load request from client
	err = client_request.load();
	if (err == -1)
	{
		close(this->client_sock);
		return -1;
	}

	// 查找是否有缓存，有的话添加If-Modified-Since字段
	if (!client_request.hostname.empty())
	{
		Cache *cache = Cache::get_instance();
		CacheEntity *entity = cache->find_entity(client_request.hostname, client_request.url);
		if (entity != nullptr) // 有缓存
		{
			client_request.headers["If-Modified-Since"] = string().assign(entity->modified_gmt);
			add_modified = true;
		}
	}

	return 0;
}

int Proxy::forward_request()
{
	target_sock = client_request.send();
	if (target_sock == -1)
	{
		close(client_sock);
		return -1;
	}
	return 0;
}

int Proxy::load_response()
{
	int err;

	target_response = Response(target_sock, client_sock);

	// load response from server
	err = target_response.load();

	if (err == -1) // 关闭两个套接字
	{
		close(this->client_sock);
		close(this->target_sock);
		return -1;
	}

	// 缓存此响应
	auto host_it = client_request.headers.find("Host");
	auto lm_it = target_response.headers.find("Last-Modified");
	if (host_it != client_request.headers.end()) // 存在Host字段
	{
		if (!strcmp(target_response.code, "200"))
		{
			if (lm_it != target_response.headers.end()
				&& target_response.body_type == Response::BodyType::ContentLength) // 可缓存
			{
				Cache *cache = Cache::get_instance();
				cache->add(host_it->second, client_request.url, (char *) lm_it->second.c_str(),
						   target_response.body, target_response.body_len);
			}
		} else if (!strcmp(target_response.code, "304")) // 可能是自己加的If-Modified-Since，需判断
		{
			if (add_modified) // If-Modified-Since是否是自己加的，修改响应状态码并插入body
			{
				strcpy(target_response.code, "200"); // 状态码更新为200
				strcpy(target_response.phrase, "OK"); // 修改状态短语（演示时可注释掉这一行）

				Cache *cache = Cache::get_instance();
				CacheEntity *entity = cache->find_entity(client_request.headers["Host"], client_request.url);
				if (entity != nullptr)
				{
					target_response.headers["Content-Length"] = std::to_string(
							entity->body_len); // 设置Content-Length响应头以描述body大小

					char filename[30]; // 保存文件名
					snprintf(filename, sizeof(filename), "cache/%zu", entity->id); // 保存在cache目录下
					FILE *f = fopen(filename, "r");
					target_response.body = new char[entity->body_len]; // 申请空间用于存储body
					fread(target_response.body, entity->body_len, 1, f);
					fclose(f);

					target_response.body_len = entity->body_len;
				} else
				{
					log_warn("cache entity missing!\n");
				}

				if (endswith(client_request.url, ".js")) // JS文件还需要添加Content-Type字段，否则浏览器会拒绝执行
					target_response.headers["Content-Type"] = "application/javascript";
			}
		}
	}

	return 0;
}

int Proxy::forward_response()
{
	int ret_sock = target_response.send();
	if (ret_sock == this->client_sock) // 成功发送，输出日志
	{
		char client_ip[16];
		inet_ntoa_r(client_request.peer_addr.sin_addr, client_ip);

		log_info("[%s:%hu] %s %s -> %s %s\n",
				 client_ip, client_request.peer_addr.sin_port,
				 client_request.method, client_request.url,
				 target_response.code, target_response.phrase
		);
	}

	close(this->client_sock);
	close(this->target_sock);
	return 0;
}
