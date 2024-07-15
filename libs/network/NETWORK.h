#ifndef NETWORK_H
#define NETWORK_H
#include <iostream>
#include <vector>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "Log.hpp"

struct IPAddr {
    union {
        uint8_t parts[4];
        uint32_t whole;
    } octets;
    uint32_t port;
};

class NetworkException final : std::exception {
public:
    explicit NetworkException(const char *description) : _description(description) {
        debugLog(_description.c_str());
    };

    [[nodiscard]] const char *what() const noexcept override {
        return _description.c_str();
    }

private:
    std::string _description;
};


namespace network {
     int conn(const IPAddr &ipAddr);
     void sendData(int sock, const std::vector<uint8_t> &buff);
     std::vector<uint8_t > readDataFromSocket(int sock);
     int createServerListenSock(int port);
     int getClientSockFromListenSock(int listenSock);
};

#endif