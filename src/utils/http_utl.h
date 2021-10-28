//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_HTTP_MSG_UTIL_H
#define HTTP_PROXY_SERVER_HTTP_MSG_UTIL_H

#include <arpa/inet.h>
#include <map>
#include <string>

/**
 * 创建一个IPv4套接字并绑定至给定的端口
 *
 * @param address
 * @param port
 * @return
 */
sockaddr_in init_sockaddr_in(char *address, uint16_t port);

/**
 *
 * @param message headers字符数组的开始
 * @return 一个pair，[0]为headers部分的长度（包括结尾的1个\r\n，不包括与body分隔的\r\n）；[1]为headers字典
 */
std::pair<size_t, std::map<std::string, std::string>> parse_headers(char *message);

/**
 * 将解析好的headers转换为字符串
 * @param headers
 * @return headers字符串，不包括与body分隔用的\r\n
 */
std::string headers2str(std::map<std::string, std::string> &headers);

/**
 * 获取一个chunk的大小
 *
 * @param p_chunk 指向该chunk的第一个字节
 * @return 成功返回chunk大小，失败返回-1
 */
ssize_t get_chunk_size(char *p_chunk);

#endif //HTTP_PROXY_SERVER_HTTP_MSG_UTIL_H
