
#ifdef SERVER_EXAMPLE

#include <iostream>
#include "../include/tcp_server.h"

using namespace std;

TcpServer server;

server_observer_t observer1, observer2;

int main()
{
    cout << "Hello World";

    pipe_ret_t startRet = server.start(65123);
    if (startRet.isSuccessful())
    {
        std::cout << "Server setup succeeded\n";
    }
    else
    {
        std::cout << "Server setup failed: " << startRet.message() << "\n";
        return EXIT_FAILURE;
    }

    return 0;
}

#endif