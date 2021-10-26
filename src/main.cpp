#include "ProxyServer.h"

using namespace std;


int main(int argc, char *argv[])
{
	ServerConfig config = ServerConfig::from_arg(argc, argv);
	ProxyServer server = ProxyServer(config);

	server.init();
	server.start();

	return 0;
}
