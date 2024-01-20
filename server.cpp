#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <Ws2spi.h>

#pragma comment(lib, "Ws2_32.lib")
#include "server.hpp"

void SN::Server::RegisterUsers()
{
    while(true)
    {
        Listen();
        if(Poll(m_socket, POLLRDNORM, 1) == 1)
        {
            auto return_tuple = Accept();
            SOCKET accepted_socket = std::get<1>(return_tuple);
            Send(accepted_socket, "what is your name?");
            std::string name;
            int result = Poll(accepted_socket, POLLRDNORM, -1);
            if(result == 1)
            {
                name = std::get<1>(Recive(accepted_socket));
                std::cout << std::endl << name << " registered" << std::endl;
            }
            else if(result == SOCKET_ERROR)
            {
                std::cerr << "RegisterUsers() failed" << std::endl;
            }
            std::tuple<SOCKET, std::string> user = {accepted_socket, name};
            m_users.emplace(user);
        }
    }
}

void SN::Server::BroadcastMessage(std::string sender, std::string message)
{
    message = sender + ": " + message;
    for(auto user : m_users)
    {
        if(sender != std::get<1>(user))
        {
            Send(std::get<0>(user) ,message.c_str());
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

            WSAPOLLFD connected_socket = {};
            connected_socket.fd = accepted_socket;
            connected_socket.events = POLLRDNORM;
            int result = WSAPoll(&connected_socket, 1, 1000);

            if(result == 1)
            {
                auto return_tuple = Recive(accepted_socket);
                std::string message = std::get<1>(return_tuple);
                std::cout << name << ": " << message << std::endl;
                BroadcastMessage(name, std::get<1>(return_tuple));
            }
        }
    }


    /*if(!from.empty())
    {
        from = " from " + from ;
    }
    char buff[100] = {};
    int result = 0;
    while(result != SOCKET_ERROR)
    {
        result = recv(socket, buff, 100, 0);
        if(result == SOCKET_ERROR)
        {
            printf("recv failed: %d\n", WSAGetLastError());
        }
        std::cout << std::endl << "message recived" << from << ": " << buff << std::endl;
        memset(buff, 0, sizeof(buff));
    }*/
}


SN::Server::Server(const char* name_or_ip, const char* service_or_port): Socket(name_or_ip, service_or_port)
{
    
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


        //std::thread recv_thread(&SN::Socket::ReciveLoop, &server, accepted_socket, name);
        //int result = 0;
        //while(true)
        //{
            //std::tuple<int, std::string> return_tuple = server.Recive(server.m_accepted_socket);
            //result = std::get<0>(return_tuple);
            //std::string message = ""; //std::get<1>(return_tuple);
            //std::cout << message << std::endl;
            //std::string response = "echo: " + message;
            //std::cin >> message;
            //server.Send(server.m_accepted_socket, message.c_str());
        //}
        recv_thread.join();
        printf("1goodbye\n");
    }
    printf("goodbye\n");

    WSACleanup();
}