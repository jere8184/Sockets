#include <iostream>
#include "socket.hpp"
#include <thread> 



#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

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
    

    SN::Socket client = SN::Socket("localhost", "999");
    client.CreateSocketAndBind();
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


    
    std::thread recv_thread(&SN::Socket::ReciveLoop, &client, client.m_socket, "");
    std::string input = "";
    while(input != "z")
    {
        getline(std::cin, input);
        client.Send(client.m_socket ,input.c_str());
    }

    Sleep(100000);
    WSACleanup();
}