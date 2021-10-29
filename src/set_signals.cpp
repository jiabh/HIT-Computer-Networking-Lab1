//
// Created by jiabh on 2021/10/27.
//

#include <csignal>
#include "set_signals.h"

void handle_pipe(int sig)
{}

void clear_sig_pipe()
{
	/**
	 * 参考链接：send或者write socket遭遇SIGPIPE信号
	 * https://www.cnblogs.com/klcf0220/ppp/5959093.html
	 */
	struct sigaction action{};
	action.sa_handler = handle_pipe;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGPIPE, &action, nullptr);
}
