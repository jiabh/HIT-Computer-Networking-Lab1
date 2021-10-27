//
// Created by jiabh on 2021/10/27.
//

#ifndef HTTP_PROXY_SERVER_SOCKET_UTL_H
#define HTTP_PROXY_SERVER_SOCKET_UTL_H


#include <netinet/in.h>

/**
 * 可重入的inet_ntoa
 *
 * @param in
 * @param res 存储地址字符串的位置。当无法转换时，该字符串将被写为255.255.255.255
 * @return 成功转换返回0，否则返回-1
 */
int inet_ntoa_r(in_addr in, char *res);

#endif //HTTP_PROXY_SERVER_SOCKET_UTL_H
