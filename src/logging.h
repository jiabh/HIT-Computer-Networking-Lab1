//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_LOGGING_H
#define HTTP_PROXY_SERVER_LOGGING_H

/**
 * 参考链接：C/C++关于printf的宏定义封装
 * https://blog.csdn.net/Poetry_And_Distance/article/details/81007604
 */

#define ESC_START     "\033["
#define ESC_END       "\033[0m"
#define COLOR_SUCCESS "32;40;1m"
#define COLOR_WARN    "33;40;1m"
#define COLOR_INFO    "36;40;1m"
#define COLOR_ERROR   "31;40;1m"

#define log_success(format, args...) (printf( ESC_START COLOR_SUCCESS "Info: " ESC_END format, ##args))

#define log_info(format, args...) (printf( ESC_START COLOR_INFO "Info: " ESC_END format, ##args))

#define log_warn(format, args...) (printf( ESC_START COLOR_WARN "Warning: " ESC_END format, ##args))

/**
 * 输出警告日志，并输出错误错误代号和描述，以及行末的换行符
 */
#define log_warn_with_msg(format, args...) (printf( ESC_START COLOR_WARN "Warning: " ESC_END format " (%d %s)\n", ##args, errno, strerror(errno)))

#define log_error(format, args...) (printf( ESC_START COLOR_ERROR "Error: " format ESC_END, ##args))

/**
 * 输出错误日志，并输出错误错误代号和描述，以及行末的换行符
 */
#define log_error_with_msg(format, args...) (printf( ESC_START COLOR_ERROR "Error: " format " (%d %s)\n" ESC_END, ##args, errno, strerror(errno)))


#endif //HTTP_PROXY_SERVER_LOGGING_H
