#include "socket.hpp"
#include <ws2tcpip.h>
#include <iostream>


SN::Socket::Socket(const char* name_or_ip, const char* service_or_port) : m_name_or_ip(name_or_ip), m_service_or_port(service_or_port){}

int SN::Socket::CreateSocketAndBind()
{
    addrinfo hints = {};
    addrinfo *address_info;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    int result = getaddrinfo(m_name_or_ip, m_service_or_port, &hints, &address_info);
    if(result != 0)
    {
        std::cout << "getaddrinfo() failed and returned: " << result <<  " WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
    }


    SOCKET new_socket = INVALID_SOCKET;
    new_socket = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);

    if (new_socket == INVALID_SOCKET) 
    {
        std::cout << "socket() failed and WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        freeaddrinfo(address_info);
        return 1;
    }

    result = bind(new_socket, address_info->ai_addr, address_info->ai_addrlen);
    freeaddrinfo(address_info);

    if(result != 0)
    {
        std::cout << "bind() failed and WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        return 1;
    }
    m_socket = new_socket;
    return 0;
}

int SN::Socket::ListenAndAccept()
{
    if (listen(m_socket, 3) != 0) 
    {
        printf( "Listen failed with error: %d\n", WSAGetLastError());
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }

    //SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET connected_socket = INVALID_SOCKET;
    connected_socket = accept(m_socket, NULL, NULL);
    if(connected_socket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }


    m_connected_sockets.push_back({"Default",connected_socket});
    return 0;
}


int SN::Socket::Connect(const char* target_name_or_ip, const char* target_service_or_port)
{
    addrinfo hints = {};
    addrinfo* addrinfo = nullptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    int result = getaddrinfo(target_name_or_ip, target_service_or_port, &hints, &addrinfo);
    if(result != 0)
    {
        std::cout << "getaddrinfo() failed and returned: " << result <<  " WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
    }
    //GetAddressInfo(target_name_or_ip, target_service_or_port, addrinfo);


    if(addrinfo != nullptr)
    {
        if(connect(m_socket, addrinfo->ai_addr, addrinfo->ai_addrlen) != 0)
        {
            printf("connect failed: %d\n", WSAGetLastError());
        }
        //freeaddrinfo(addrinfo);
        return 0;
    }
    return 1;
}

addrinfo* SN::Socket::GetAddressInfo(const char* name_or_ip, const char* service_or_port, addrinfo*& address_info)
{
    addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    int result = getaddrinfo(name_or_ip, service_or_port, &hints, &address_info);
    if(result != 0)
    {
        std::cout << "getaddrinfo() failed and returned: " << result <<  " WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        return nullptr;
    }
    return address_info;
}

int SN::Socket::Send(SOCKET socket, const char* message)
{
    int result = send(socket, message, strlen(message) + 1, 0);
    if(result == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
    }
    return result;
}


std::tuple<int, std::string> SN::Socket::Recive(SOCKET socket)
{
    char buff[100] = {};
    int result = recv(socket, buff, 100, 0);

    if(result == SOCKET_ERROR)
    {
        printf("recv failed: %d\n", WSAGetLastError());
    }

    return {result, std::string(buff)};
}

void SN::Socket::ReciveLoop(SOCKET socket, std::string from)
{
    if(!from.empty())
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
    }
}