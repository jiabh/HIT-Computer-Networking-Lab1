//
// Created by jiabh on 2021/10/26.
//

#include <cstring>
#include <unistd.h>
#include <map>
#include "HTTPMessage.h"

HTTPMessage::HTTPMessage(int from_sock, int to_sock)
{
	this->from_sock = from_sock;
	this->to_sock = to_sock;
}

HTTPMessage::~HTTPMessage()
{
	delete this->buffer;
	delete this->body;
}
