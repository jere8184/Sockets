
#include "server.hpp"

#include <iostream>
#include <thread>


void SN::Server::RegisterUsers()
{
    while(true)
    {
        this->Listen();
        if(Poll(m_socket, POLLRDNORM, 1) == 1)
        {
            auto [result, accepted_socket] = Accept();
            Send(accepted_socket, "what is your name?");
            result = Poll(accepted_socket, POLLRDNORM, -1);
            if(result == 1)
            {
                std::string name = Recive(accepted_socket).second;
                std::cout << std::endl << name << " registered" << std::endl;
                std::pair<SOCKET, std::string> user = {accepted_socket, name};
                m_users.emplace(user);
            }
            else if(result == SOCKET_ERROR || result == POLLHUP)
            {
                std::cerr << "RegisterUsers() failed" << std::endl;
            }
        }
    }
}

void SN::Server::BroadcastMessage(std::string sender, std::string message)
{
    message = sender + ": " + message;
    for(auto user : m_users)
    {
        if(sender != user.second)
        {
            Send(user.first ,message.c_str());
        }
    }
}

void SN::Server::ReciveFromUsersLoop()
{
    while(true)
    {
        for(auto user: m_users)
        {
            std::string name = std::get<1>(user);
            SOCKET accepted_socket = std::get<0>(user);
            int result = Poll(accepted_socket, POLLRDNORM, 1000);
            if(result == 1)
            {
                auto [result, message] = Recive(accepted_socket);
                std::cout << name << ": " << message << std::endl;
                BroadcastMessage(name, message);
            }
            if(result == POLLHUP)
            {
                std::cout << name << " has disconnected" << std::endl;
                m_users.erase(user);
            }
        }
    }
}


SN::Server::Server(const char* name_or_ip, const char* service_or_port): Socket(name_or_ip, service_or_port){}


SN::Socket server = SN::Socket("localhost", "999");

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    closesocket(server.m_socket);
    WSACleanup();
    exit(signum); 
    return; 
}

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(result == 0)
    {
        std::cout << "winsock intialised: success" << std::endl; 
    }
    else
    {
        std::cout << "winsock intialised: failed with error code " << result << std::endl; 
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
    {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return 1;
    }
    else
    {
        printf("The Winsock 2.2 dll was found okay\n");
    }
    

    SN::Server server = SN::Server("localhost", "888");
    server.CreateSocket();
    server.BindSocket();
    server.EnableNonBlocking();

    if(server.m_socket != INVALID_SOCKET)
    {
        std::thread register_thread(&SN::Server::RegisterUsers, &server);

        for(int count = 0; count < 200; count++)
        {
            Sleep(1000);
            std::cout << "waiting for connection: " << count << std::endl;
            if(!server.m_users.empty())
            {
                break;
            }
        }

        std::thread recv_thread(&SN::Server::ReciveFromUsersLoop, &server);
        recv_thread.join();
        register_thread.join();
    }
    printf("goodbye\n");

    WSACleanup();
}