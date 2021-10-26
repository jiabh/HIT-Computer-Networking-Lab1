//
// Created by jiabh on 2021/10/26.
//


#include <iostream>
#include <unistd.h>
#include "Proxy.h"
#include "http_message/Request.h"
#include "http_message/Response.h"


Proxy::Proxy(int client_sock)
{
	this->client_sock = client_sock;
	this->target_sock = -1;
}

void Proxy::run()
{
	Request client_request(this->client_sock);
	Response target_response(-1, -1);

	// load request from client
	int err = client_request.load();
	if (err)
		goto quit;

	// send the request
	this->target_sock = client_request.send();
	if (this->target_sock == -1)
		goto quit;

	target_response = Response(this->target_sock, this->client_sock);
	// load response from server
	target_response.load();

	// send the response back to the client
	target_response.send();

	quit:
	close(this->client_sock);
}
