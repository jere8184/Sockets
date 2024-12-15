#include <set>
#include <utility>
#include <string>

#include "socket.hpp"

namespace SN
{
    class Server : public Socket
    {
    public:
        std::set<std::pair<SOCKET, std::string>> m_users;
        void RegisterUsers();
        Server(const char *name_or_ip = nullptr, const char *service_or_port = "888");
        void BroadcastMessage(std::string sender, std::string message);
        void ReciveFromUsersLoop();
    };
}
