
#ifdef SERVER_EXAMPLE

#include <iostream>
#include "../include/tcp_server.h"

using namespace std;

TcpServer server;

server_observer_t observer1, observer2;

void onIncomingMsg1(const std::string &clientIP, const char *msg, size_t size)
{
    std::string msgStr = msg;
    std::cout << "Observer1 got client msg: " << msgStr << "\n";
}
void onIncomingMsg2(const std::string &clientIP, const char *msg, size_t size)
{
    std::string msgStr = msg;
    // print client message
    std::cout << "Observer2 got client msg: " << msgStr << "\n";
}

void onClientDisconnected(const std::string &ip, const std::string &msg)
{
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << "\n";
}

void acceptClient()
{
    try
    {
        std::cout << "waiting for incoming client...\n";
        std::string clientIP = server.acceptClient(0);
        std::cout << "accepted new client with IP: " << clientIP << "\n"
                  << "== updated list of accepted clients ==" << "\n";
        server.printClients();
    }
    catch (const std::runtime_error &error)
    {
        std::cout << "Accepting client failed: " << error.what() << "\n";
    }
}

void printMenu()
{
    std::cout << "\n\nselect one of the following options: \n"
              << "1. send all clients a message\n"
              << "2. print list of accepted clients\n"
              << "3. send message to a specific client\n"
              << "4. close server and exit\n";
}

int getMenuSelection()
{
    int selection = 0;
    std::cin >> selection;
    if (!std::cin)
    {
        throw std::runtime_error("invalid menu input. expected a number, but got something else");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return selection;
}

bool handleMenuSelection(int selection)

{
    static const int minSelection = 1;
    static const int maxSelection = 4;
    if (selection < minSelection || selection > maxSelection)
    {
        return true;
    }
    return false;
}

int main()
{


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

    observer1.incomingPacketHandler = onIncomingMsg1;
    observer1.disconnectionHandler = onClientDisconnected;
    observer1.wantedIP = "127.0.0.1";

    server.subscribe(observer1);

    acceptClient();

    bool shouldTerminate = false;
    while (!shouldTerminate)
    {
        printMenu();
        int selection = getMenuSelection();
        shouldTerminate = handleMenuSelection(selection);
    }

    return 0;
}

#endif