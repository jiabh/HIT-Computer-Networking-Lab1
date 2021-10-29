//
// Created by jiabh on 2021/10/29.
//

#ifndef HTTP_PROXY_SERVER_CACHEENTITY_H
#define HTTP_PROXY_SERVER_CACHEENTITY_H

#include <string>
#include <functional>
#include <ctime>
#include <cstring>

using std::string;

/**
 * 缓存中的一个条目
 */
class CacheEntity
{
public:
	size_t id;                      // 缓存唯一id
	const string hostname;          // 域名
	const string url;               // URL
	size_t body_len;                // 文件长度（用于读取）
	char modified_gmt[30] = {'\0'}; // 缓存时间
protected:
	std::hash<string> hash_str;

public:
	/**
	 * @param hostname 域名
	 * @param url URL
	 * @param modified_gmt Modified-Since字段的值
	 * @param body_len body长度
	 */
	CacheEntity(string &hostname, string &url, char *modified_gmt, size_t body_len) :
			hostname(hostname), url(url), body_len(body_len)
	{
		this->id = hash_str(hostname + url);
		strcpy(this->modified_gmt, modified_gmt);
	}
};


#endif //HTTP_PROXY_SERVER_CACHEENTITY_H
