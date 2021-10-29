//
// Created by jiabh on 2021/10/28.
//

#ifndef HTTP_PROXY_SERVER_CACHE_H
#define HTTP_PROXY_SERVER_CACHE_H

#include <ctime>
#include <map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "CacheEntity.h"

using std::map;
using std::vector;
using std::string;

class Cache
{
private:
	static Cache *_instance; // 指向单例对象
	static std::mutex _mtx;

	vector<CacheEntity> _cache_map; // 缓存map

public:
	/**
	 * 获取缓存的实例
	 *
	 * @return 缓存对象的指针
	 */
	static Cache *get_instance();

	/**
	 * 添加（或更新）一个缓存
	 *
	 * @param hostname 主机名
	 * @param url HTTP请求报文中的URL
	 * @param modified_gmt Modified-Since字段
	 * @param body 消息体首字节指针
	 * @param body_len 消息体长度
	 * @return 更新成功返回0，否则返回-1
	 */
	int add(string hostname, string url, char *modified_gmt, char *body, size_t body_len);

	/**
	 * 根据host和url查找缓存条目
	 * @param hostname 域名
	 * @param url URL
	 * @return 缓存条目的指针；如果无缓存，则返回nullptr
	 */
	CacheEntity *find_entity(const string &hostname, const string &url);

	/**
	 * 加载缓存内容
	 *
	 * @param id 缓存条目的id
	 * @param buffer 加载位置
	 * @return 成功返回0；条目不存在返回-1
	 */
//	int load(size_t id, char *buffer);

private:
	Cache() = default;
};

#endif //HTTP_PROXY_SERVER_CACHE_H
