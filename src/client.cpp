
#include "socket.hpp"

#include <thread>
#include <csignal>
#include <iostream>

SN::Socket client = SN::Socket("localhost", "999");

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    closesocket(client.m_socket);
    WSACleanup();
    exit(signum); 
    return; 
}


int main()
{
    signal(SIGINT, signalHandler);  

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
    

    client.CreateSocket();
    if(client.m_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return 1;
    }

    if(client.Connect("localhost", "888") != 0)
    {
        WSACleanup();
        return 1;
    }

    //if(client.Send(client.m_socket, "hello world") == SOCKET_ERROR)
    //{
    //    WSACleanup();
    //    return 1;
    //}


    
    std::thread recv_thread(&SN::Socket::ReciveLoop, &client);
    std::thread send_thread(&SN::Socket::SendLoop, &client);

    send_thread.join();
    recv_thread.join();

    WSACleanup();
}