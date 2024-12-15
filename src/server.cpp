
#include "server.hpp"

#include <iostream>
#include <thread>

void SN::Server::RegisterUsers()
{
    while (true)
    {
        this->Listen();
        if (Poll(m_socket, POLLRDNORM, 1) == 1)
        {
            auto [result, accepted_socket] = Accept();
            Send(accepted_socket, "what is your name?");
            result = Poll(accepted_socket, POLLRDNORM, -1);
            if (result == 1)
            {
                std::string name = Recive(accepted_socket).second;
                std::cout << std::endl
                          << name << " registered" << std::endl;
                std::pair<SOCKET, std::string> user = {accepted_socket, name};
                this->users.emplace(user);
            }
            else if (result == SOCKET_ERROR || result == POLLHUP)
            {
                std::cerr << "RegisterUsers() failed" << std::endl;
            }
        }
    }
}

void SN::Server::BroadcastMessage(std::string sender, std::string message)
{
    message = sender + ": " + message;
    for (auto user : this->users)
    {
        if (sender != user.second)
        {
            Send(user.first, message.c_str());
        }
    }
}

void SN::Server::ReciveFromUsersLoop()
{
    while (true)
    {
        for (auto user : this->users)
        {
            std::string name = std::get<1>(user);
            SOCKET accepted_socket = std::get<0>(user);
            int result = Poll(accepted_socket, POLLRDNORM, 1000);
            if (result == 1)
            {
                auto [result, message] = Recive(accepted_socket);
                std::cout << name << ": " << message << std::endl;
                BroadcastMessage(name, message);
            }
            if (result == POLLHUP)
            {
                std::cout << name << " has disconnected" << std::endl;
                this->users.erase(user);
            }
        }
    }
}

SN::Server::Server(const char *name_or_ip, const char *service_or_port) : Socket(name_or_ip, service_or_port) {}

SN::Socket *server_ptr = nullptr;

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    if (server_ptr)
        server_ptr->CloseSocket();
    SN::Socket::CleanUp();
    exit(signum);
    return;
}

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        std::cerr << "server: expects two arguments (host and port)";
    }

    if (!SN::Socket::Init())
    {
        return 1;
    }

    SN::Server server = SN::Server(argv[1], argv[2]);

    server_ptr = &server;

    server.CreateSocket();
    server.BindSocket();
    server.EnableNonBlocking();

    if (server.m_socket != INVALID_SOCKET)
    {
        std::thread register_thread(&SN::Server::RegisterUsers, &server);

        for (int count = 0; count < 200; count++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "waiting for connection: " << count << std::endl;
            if (!server.GetUsers().empty())
            {
                break;
            }
        }

        std::thread recv_thread(&SN::Server::ReciveFromUsersLoop, &server);
        recv_thread.join();
        register_thread.join();
    }
    server_ptr = nullptr;
    printf("goodbye\n");
    SN::Socket::CleanUp();
}