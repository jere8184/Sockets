#pragma once
#include <winsock2.h>
#include <tuple>
#include <string>
#include <vector>
#include <tuple>
#include <ws2tcpip.h>


namespace SN
{
    class Socket
    {

        public:
            Socket(const char* name_or_ip = nullptr, const char* service_or_port = "888");
            SOCKET m_socket = INVALID_SOCKET;
            std::vector<std::tuple<std::string, SOCKET>> m_connected_sockets = {};
            int Poll(SOCKET& socket, ULONG  option, int time_out);
            int Listen();
            std::tuple<int, SOCKET> Accept();
            int CreateSocket();
            int BindSocket();
            int EnableNonBlocking();
            int Connect(const char* target_name_or_ip, const char* target_service_or_port);
            int Send(SOCKET socket, const char* message);
            std::tuple<int, std::string> Recive(SOCKET socket);
            void ReciveLoop(SOCKET socket);


        private:
            const char* m_name_or_ip = nullptr;
            const char* m_service_or_port = "";
            static addrinfo* GetAddressInfo(const char* name_or_ip, const char* service_or_port, addrinfo*& address_info);
            addrinfo *m_address_info;
    };
};