#pragma once

#include <winsock2.h>

#include <string>
#include <vector>

namespace SN
{
    class Socket
    {

    public:
        Socket(const char *name_or_ip = nullptr, const char *service_or_port = nullptr);
        SOCKET m_socket = INVALID_SOCKET;
        int Poll(SOCKET socket, ULONG option, int time_out);
        int Listen();
        std::pair<int, SOCKET> Accept();
        int CreateSocket();
        int BindSocket();
        int EnableNonBlocking();
        int Connect(const char *target_name_or_ip, const char *target_service_or_port);
        int Send(SOCKET reciver, const char *message);
        std::pair<int, std::string> Recive(SOCKET sender);
        void ReciveLoop();
        void SendLoop();
        void CloseSocket();
        static bool InitWinSock();
        static void CleanUp();

    private:
        const char *m_name_or_ip = nullptr;
        const char *m_service_or_port = "";
        static addrinfo *GetAddressInfo(const char *name_or_ip, const char *service_or_port, addrinfo *&address_info);
        addrinfo *m_address_info;
    };
};