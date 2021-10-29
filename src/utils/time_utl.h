//
// Created by jiabh on 2021/10/29.
//

#ifndef HTTP_PROXY_SERVER_TIME_UTL_H
#define HTTP_PROXY_SERVER_TIME_UTL_H

#include <ctime>

/**
 * 将tm类型的时间转换为标准GMT字符串
 *
 * @param p_time 指向tm类型
 * @param buffer 字符串缓冲区，需要30个字节
 */
void tm2gmt(const tm *p_time, char *buffer);

#endif //HTTP_PROXY_SERVER_TIME_UTL_H
