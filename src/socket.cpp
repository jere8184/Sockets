#include "socket.hpp"

#include <ws2tcpip.h>
#include <iostream>

SN::Socket::Socket(const char *name_or_ip, const char *service_or_port) : m_name_or_ip(name_or_ip), m_service_or_port(service_or_port) {}

int SN::Socket::Poll(SOCKET socket, ULONG option, int time_out)
{
    WSAPOLLFD poll_socket = {};
    poll_socket.fd = socket;
    poll_socket.events = option;
    int result = WSAPoll(&poll_socket, 1, time_out);
    if (poll_socket.revents & POLLHUP)
    {
        return POLLHUP;
    }
    else if (result == SOCKET_ERROR)
    {
        std::cout << "WSAPoll: " << WSAGetLastError() << std::endl;
        return SOCKET_ERROR;
    }
    return result;
}

int SN::Socket::Listen()
{
    if (listen(m_socket, 3) != 0)
    {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        this->CloseSocket();
        this->CleanUp();
        return 1;
    }
    return 0;
}

std::pair<int, SOCKET> SN::Socket::Accept()
{
    SOCKET connected_socket = INVALID_SOCKET;
    connected_socket = accept(m_socket, NULL, NULL);
    if (connected_socket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        this->CloseSocket();
        this->CleanUp();
        return {1, INVALID_SOCKET};
    }
    return {0, connected_socket};
}

int SN::Socket::CreateSocket()
{
    GetAddressInfo(m_name_or_ip, m_service_or_port, m_address_info);

    SOCKET new_socket = INVALID_SOCKET;
    new_socket = socket(m_address_info->ai_family, m_address_info->ai_socktype, this->m_address_info->ai_protocol);

    if (new_socket == INVALID_SOCKET)
    {
        std::cout << "socket() failed and WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        freeaddrinfo(m_address_info);
        return 1;
    }
    m_socket = new_socket;
    return 0;
}

int SN::Socket::BindSocket()
{
    int result = bind(m_socket, m_address_info->ai_addr, m_address_info->ai_addrlen);
    std::cout << m_address_info->ai_addr << std::endl;
    freeaddrinfo(m_address_info);

    if (result != 0)
    {
        std::cout << "bind() failed and WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        return 1;
    }
    return 0;
}

int SN::Socket::EnableNonBlocking()
{
    u_long mode = 1;
    int result = ioctlsocket(m_socket, FIONBIO, &mode);
    if (result != 0)
    {
        std::cout << "ioctlsocket failed with error: " << result << std::endl;
    }
    return result;
}

int SN::Socket::Connect(const char *target_name_or_ip, const char *target_service_or_port)
{
    addrinfo hints = {};
    addrinfo *addrinfo = nullptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    this->GetAddressInfo(target_name_or_ip, target_service_or_port, addrinfo);

    if (addrinfo != nullptr)
    {
        if (connect(m_socket, addrinfo->ai_addr, addrinfo->ai_addrlen) == SOCKET_ERROR)
        {
            printf("connect failed: %d\n", WSAGetLastError());
            return SOCKET_ERROR;
        }
    }
    return 0;
}

addrinfo *SN::Socket::GetAddressInfo(const char *name_or_ip, const char *service_or_port, addrinfo *&address_info)
{
    addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int result = getaddrinfo(name_or_ip, service_or_port, &hints, &address_info);
    if (result != 0)
    {
        std::cout << "getaddrinfo() failed and returned: " << result << " WSAGetLastError() returned: " << WSAGetLastError() << std::endl;
        return nullptr;
    }
    return address_info;
}

int SN::Socket::Send(SOCKET sender, const char *message)
{
    int result = send(sender, message, strlen(message) + 1, 0);
    if (result == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
    }
    return result;
}

std::pair<int, std::string> SN::Socket::Recive(SOCKET sender)
{
    char buff[100] = {};
    int result = recv(sender, buff, 100, 0);

    if (result == SOCKET_ERROR)
    {
        printf("recv failed: %d\n", WSAGetLastError());
    }

    return {result, std::string(buff)};
}

void SN::Socket::ReciveLoop()
{
    char buff[100] = {};
    int result = 0;
    while (result != SOCKET_ERROR)
    {
        result = recv(this->m_socket, buff, 100, 0);
        if (result == SOCKET_ERROR)
        {
            printf("recv failed: %d\n", WSAGetLastError());
        }
        std::cout << std::endl
                  << buff << std::endl;
    }
}

void SN::Socket::SendLoop()
{
    std::string input = "";
    while (input != "z")
    {
        getline(std::cin, input);
        if (std::cin.fail() || std::cin.eof())
        {
            std::cin.clear(); // reset cin state
            break;
        }
        this->Send(this->m_socket, input.c_str());
    }
}

void SN::Socket::CloseSocket()
{
    closesocket(this->m_socket);
}

bool SN::Socket::InitWinSock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result == 0)
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
        return false;
    }
    else
    {
        printf("The Winsock 2.2 dll was found okay\n");
        return true;
    }
}

void SN::Socket::CleanUp()
{
    WSACleanup();
}
