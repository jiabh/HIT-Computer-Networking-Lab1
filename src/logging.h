//
// Created by jiabh on 2021/10/26.
//

#ifndef HTTP_PROXY_SERVER_LOGGING_H
#define HTTP_PROXY_SERVER_LOGGING_H

#define ESC_START     "\033["
#define ESC_END       "\033[0m"
#define COLOR_SUCCESS "32;40;1m"
#define COLOR_WARN    "33;40;1m"
#define COLOR_INFO    "36;40;1m"
#define COLOR_ERROR   "31;40;1m"

#define log_success(format, args...) (printf( ESC_START COLOR_SUCCESS "Info: " ESC_END format, ##args))

#define log_info(format, args...) (printf( ESC_START COLOR_INFO "Info: " ESC_END format, ##args))

#define log_warn(format, args...) (printf( ESC_START COLOR_WARN "Warning: " ESC_END format, ##args))

#define log_error(format, args...) (printf( ESC_START COLOR_ERROR "Error: " format ESC_END, ##args))


#endif //HTTP_PROXY_SERVER_LOGGING_H
