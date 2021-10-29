//
// Created by jiabh on 2021/10/28.
//

#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include <cstring>
#include "../logging.h"
#include "Cache.h"

using std::mutex;
using std::lock_guard;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

Cache *Cache::_instance = nullptr;
mutex Cache::_mtx;

Cache *Cache::get_instance()
{
	/**
	 * 参考链接：C++完美单例模式
	 * https://www.jianshu.com/ppp/69eef7651667
	 */
	lock_guard<mutex> lock(_mtx);
	if (_instance == nullptr)
		_instance = new Cache();
	return _instance;
}

int Cache::add(string hostname, string url, char *modified_gmt, char *body, size_t body_len)
{
	std::lock_guard<std::mutex> lg(_mtx);

	// 查找缓存是否已存在
	auto it = _cache_map.begin();
	while (it != _cache_map.end())
	{
		if (it->hostname == hostname && it->url == url)
			break;
		it++;
	}

	size_t entity_id;
	if (it == _cache_map.end()) // 无缓存
	{
		CacheEntity entity(hostname, url, modified_gmt, body_len);
		_cache_map.emplace_back(entity);
		entity_id = entity.id;
	} else // 已缓存
	{
		it->body_len = body_len;
		entity_id = it->id;
	}

	// 保存body
	char filename[30]; // 保存文件名
	snprintf(filename, sizeof(filename), "cache/%zu", entity_id); // 保存在cache目录下
	FILE *f = fopen(filename, "w");
	fwrite(body, body_len, 1, f);
	fclose(f);

	return 0;
}

CacheEntity *Cache::find_entity(const string &hostname, const string &url)
{
	auto it = _cache_map.begin();
	while (it != _cache_map.end())
	{
		if (it->hostname == hostname && it->url == url)
			return it.base();
		it++;
	}
	return nullptr;
}
