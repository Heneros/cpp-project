#ifdef CLIENT_EXAMPLE
#include <csignal>
#include "../include/tcp_client.h"

TcpClient client;

void sig_exit(int s)
{
    std::cout << "Closing client...\n";
    pipe_ret_t finishRet = client.close();
    if (finishRet.isSuccessful())
    {
        std::cout << "Client closed.\n";
    }
    else
    {
        std::cout << "Failed to close client.\n";
    }
    exit(0);
}

void onIncomingMsg(const char *msg, size_t size)
{
    std::cout << "Got msg from server: " << msg << "\n";
}

void onDisconnection(const pipe_ret_t &ret)
{
    std::cout << "Server disconnected: " << ret.message() << "\n";
}

void printMenu()
{
    std::cout << "select one of the following options: \n"
              << "1. send message to server\n"
              << "2. close client and exit\n";
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

int handleMenuSelection(int selection)
{
    static const int minSelection = 1;
    static const int maxSelection = 2;
    if (selection < minSelection || selection > maxSelection)
    {
        std::cout << "invalid selection: " << selection << ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
    }
    switch (selection)
    {
    case 1:
    {
        std::cout << "enter message to send:\n";
        std::string message;
        std::cin >> message;
        pipe_ret_t sendRet = client.sendMsg(message.c_str(), message.size());
        if (!sendRet.isSuccessful())
        {
            std::cout << "Failed to send message: " << sendRet.message() << "\n";
        }
        else
        {
            std::cout << "message was sent successfuly\n";
        }
        break;
    }
    case 2:
    { // close client
        const pipe_ret_t closeResult = client.close();
        if (!closeResult.isSuccessful())
        {
            std::cout << "closing client failed: " << closeResult.message() << "\n";
        }
        else
        {
            std::cout << "closed client successfully\n";
        }
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
    signal(SIGINT, sig_exit);

    return 0;
}

#endif