
#pragma once

#include <set>
#include <utility>
#include <string>
#include <optional>

#include "socket.hpp"

namespace SN
{
    class Server : public Socket
    {
    public:
        void RegisterUsers();
        Server(const char *name_or_ip = nullptr, const char *service_or_port = "888");
        void BroadcastMessage(std::string sender, std::string message);
        void ReciveFromUsersLoop();
        const std::set<std::pair<SOCKET, std::string>> &GetUsers() { return this->users; }

    private:
        std::set<std::pair<SOCKET, std::string>> users;
    };
}
