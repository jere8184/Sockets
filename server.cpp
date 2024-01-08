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
    

    SN::Socket server = SN::Socket("localhost", "888");
    server.CreateSocketAndBind();

    if(server.m_socket != INVALID_SOCKET)
    {
        std::thread listen_thread(&SN::Socket::ListenAndAccept, &server);
        for(int count = 0; count < 200; count++)
        {
            Sleep(1000);
            std::cout << "listening: " << count << std::endl;
            if(!server.m_connected_sockets.empty())
            {
                listen_thread.join();
                break;
            }
        }

        std::string& name = std::get<0>(server.m_connected_sockets[0]);
        SOCKET& connected_socket = std::get<1>(server.m_connected_sockets[0]);
        
        server.Send(connected_socket ,"what is your name?");
        name = std::get<1>(server.Recive(connected_socket));

        std::thread recv_thread(&SN::Socket::ReciveLoop, &server, connected_socket, name);


        std::string input = "";
        while(input != "z")
        {
            getline(std::cin, input);
            server.Send(connected_socket ,input.c_str());
        }

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
        printf("goodbye\n");
    }
    WSACleanup();
}