//
// Created by jiabh on 2021/10/27.
//

#include <cstdio>
#include "socket_utl.h"

int inet_ntoa_r(in_addr addr, char *res)
{
	/**
	 * 参考链接：inet_ntoa函数线程不安全
	 * https://blog.csdn.net/jakejohn/article/details/79825134
	 */
	auto *p = (unsigned char *) &(addr.s_addr);
	sprintf(res, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);

	return 0;
}
