#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#include "include/vars.h"

using namespace std;

int main(int argc, char **argv)
{
    //// fd — File Descriptor
    // SOCK_STREAM Provides sequenced, reliable, two - way, connection - based byte streams.An out - of - band data transmission mechanism may be supported.
    int server_fd = socket(AF_INET6, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in6 addr{};

    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(8080);
    // if (inet_pton(AF_INET6, argv[1], &addr.sin6_addr) <= 0)
    // {
    //     fprintf(stderr, "inet_pton error for %s: not a valid IPv4 address\n", argv[1]);
    //     close(server_fd);
    //     return EXIT_FAILURE;
    // }

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "bind");
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 5) == -1)
    {
        fprintf(stderr, "listen");
        return EXIT_FAILURE;
    }
    cout << "Server listening on port 8080...\n";
    while (true)
    {
        sockaddr_in6 client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);

        if (client_fd == -1)
        {
            perror("accept");
            return 1;
        }

        char buffer[BUFFSIZE] = {0};

        ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes > 0)
        {
            std::cout << "Received: " << buffer << "\n";
        }

        std::string response = "Hello From Server " + std::to_string(bytes) + " bytes";

        
        write(client_fd, response.c_str(), response.size());

        close(client_fd);
        cout << "Client disconnected.\n";
    }

    close(server_fd);
    return 0;
}
