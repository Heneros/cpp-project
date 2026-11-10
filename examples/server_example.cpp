
#ifdef SERVER_EXAMPLE

#include <iostream>
#include "../include/tcp_server.h"

using namespace std;

TcpServer server;

server_observer_t observer1, observer2;
std::atomic<bool> keepRunning{true};

void printMenu()
{
    std::cout << "\n\nselect one of the following options: \n"
              << "1. send all clients a message\n"
              << "2. print list of accepted clients\n"
              << "3. send message to a specific client\n"
              << "4. close server and exit\n";
    std::cout.flush();
}

void onIncomingMsg1(const std::string &clientIP, const char *msg, size_t size)
{
    std::string msgStr(msg, size);
    std::cout << "Observer1 got client msg: " << msgStr << "\n";
    printMenu();
}
void onIncomingMsg2(const std::string &clientIP, const char *msg, size_t size)
{
    std::string msgStr(msg, size);
    std::cout << "Observer2 got client msg: " << msgStr << "\n";
}

void onClientDisconnected(const std::string &ip, const std::string &msg)
{
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << "\n";
    printMenu();
}
void acceptClientsLoop()
{
    while (keepRunning.load())
    {
        try
        {
            std::string clientIP = server.acceptClient(1000);
            if (!clientIP.empty())
                std::cout << "\n[SERVER] New client accepted: " << clientIP << "\n";
            server.printClients();
            printMenu();
        }
        catch (const std::runtime_error &error)
        {
            std::string err = error.what();
            if (err.find("Timeout") == std::string::npos)
            {
                std::cout << "[SERVER] Accepting client failed: " << err << "\n";
            }
        }
    }
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

int getMenuSelection()
{
    int selection = 0;
    std::cin >> selection;
    if (!std::cin)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n');
        throw std::runtime_error("invalid menu input. expected a number, but got something else");
        return -1;
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
        std::cout << "invalid selection: " << selection << ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
    }
    switch (selection)
    {
    case 1:
    {
        std::string msg;
        std::cout << "type message to send to all connected clients:\n";
        getline(std::cin, msg);
        if (msg.empty())
        {
            std::cout << "Field empty";
            break;
        }
        cout << msg;

        auto sendingResult = server.sendToAllClients(msg.c_str(), msg.size());
        if (sendingResult.has_value())
        {
            std::cout << "sent message to all clients successfully\n";
        }
        else
        {
            std::cout << "failed to sent message: " << sendingResult.error() << "\n";
        }
        break;
    }
    case 2:
    {
        server.printClients();
        break;
    }
    case 3:
    {
        std::cout << "enter client IP:\n";
        std::string clientIP;
        std::getline(std::cin, clientIP);
        std::cout << "enter message to send:\n";
        std::string message;
        std::getline(std::cin, message);

        const auto result = server.sendToClient(clientIP, message.c_str(), message.size());
        if (!result.has_value())
        {
            std::cout << "sending failed: " << result.error() << "\n";
            // return std::unexpected(" Client " + clientIP + " not found ");
        }
        else
        {
            std::cout << "sending succeeded\n";
        }
        break;
    }
    case 4:
    {
        std::cout << "Closing server";
        keepRunning.store(false);
        server.close();
        return true;
    }
    default:
    {
        std::cout << "invalid selection: " << selection << ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
    }
    }
    return false;
}

int main()
{

     auto startRet = server.start(65123, 10, true);
    if (!startRet.has_value())
    {
        std::cout << "Error loading server: " << startRet.error() << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "Server setup succeeded on port 65123\n";

    observer1.incomingPacketHandler = onIncomingMsg1;
    observer1.disconnectionHandler = onClientDisconnected;
    observer1.wantedIP = "";
    server.subscribe(observer1);

    std::thread acceptThread(acceptClientsLoop);

    bool shouldTerminate = false;
    while (!shouldTerminate)
    {
        printMenu();
        int selection = getMenuSelection();
        if (selection != -1)
        {
            shouldTerminate = handleMenuSelection(selection);
        }
    }
    if (acceptThread.joinable())
    {
        acceptThread.join();
    }
    std::cout << "Server stopped \n";
    return 0;
}

#endif