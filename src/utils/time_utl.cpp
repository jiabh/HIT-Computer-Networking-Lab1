//
// Created by jiabh on 2021/10/29.
//

#include "time_utl.h"

#include <ctime>

void tm2gmt(const tm *p_time, char *buffer)
{
	strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", p_time);
}
