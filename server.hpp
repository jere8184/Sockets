#include <set>
#include <iostream>
#include "socket.hpp"
#include <thread>

namespace SN
{
    class Server : public Socket
    {
        public:
        std::set<std::tuple<SOCKET, std::string>> m_users;
        void RegisterUsers();
        Server(const char* name_or_ip = nullptr, const char* service_or_port = "888");
        void BroadcastMessage(std::string sender, std::string message);
        void ReciveFromUsersLoop();
    };
}

