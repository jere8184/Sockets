
#include "socket.hpp"

#include <thread>
#include <csignal>
#include <iostream>

SN::Socket *client_ptr;

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    client_ptr->CloseSocket();
    SN::Socket::CleanUp();
    exit(signum);
    return;
}

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        std::cerr << "client: expects two arguments (server hostname/IP and sever port number)";
    }

    signal(SIGINT, signalHandler);

    if (!SN::Socket::Init())
    {
        return 1;
    }

    SN::Socket client("localhost");
    client_ptr = &client;

    client.CreateSocket();
    if (client.m_socket == INVALID_SOCKET)
    {
        SN::Socket::CleanUp();
        return 1;
    }

    if (client.Connect(argv[1], argv[2]) != 0)
    {
        SN::Socket::CleanUp();
        return 1;
    }

    std::thread recv_thread(&SN::Socket::ReciveLoop, &client);
    std::thread send_thread(&SN::Socket::SendLoop, &client);

    send_thread.join();
    recv_thread.join();

    SN::Socket::CleanUp();
    client_ptr = nullptr;
}