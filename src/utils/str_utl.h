//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_STR_UTL_H
#define HTTP_PROXY_SERVER_STR_UTL_H

#include <vector>
#include <string>

using std::vector;
using std::string;

/**
 * 使用KMP算法搜索模式串，找到模式串出现的第一个位置
 *
 * @param S 源字符串
 * @param len_S 源字符串长度
 * @param T 模式串
 * @param len_T 模式串长度
 * @return 模式串在源字符串中出现的第一个位置，没有返回-1
 */
ssize_t KMP(const char *S, size_t len_S, const char *T, size_t len_T);

/**
 * 分隔字符串
 *
 * @param src 源字符串
 * @param delimiter 分隔符
 * @param eos end of string
 * @return 一个pair，[0]为以eos做结尾标志时src字符串的长度；[1]为分隔字符串下标组成的vector
 */
std::pair<size_t, std::vector<size_t>> split_str(char *src, const char *delimiter, const char *eos);

/**
 * 移除字符串头尾指定的字符串
 * @param s 被移除的字符串
 * @param pattern 指定的字符串（默认为空格）
 * @return 移除后的字符串
 */
std::string &trim(std::string &s, const std::string &pattern = " ");

#endif //HTTP_PROXY_SERVER_STR_UTL_H
