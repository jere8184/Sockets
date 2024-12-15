

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <errno.h>
#include <poll.h>

using SOCKET = int;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

#else
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#endif


#include <string>
#include <vector>

namespace SN
{
    class Socket
    {

    public:
        Socket(const char *name_or_ip = nullptr, const char *service_or_port = nullptr);
        SOCKET m_socket = INVALID_SOCKET;
        int Poll(SOCKET socket, unsigned long option, int time_out);
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
        static bool Init();
        static void CleanUp();

    private:
        const char *name_or_ip = nullptr;
        const char *service_or_port = "";
        static addrinfo *GetAddressInfo(const char *name_or_ip, const char *service_or_port, addrinfo *&address_info);
        addrinfo *address_info;
    };
};