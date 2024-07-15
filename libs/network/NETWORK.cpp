#include "NETWORK.h"

namespace network {

    int conn(const IPAddr &ipAddr) {
        const int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            throw NetworkException("Error creation socket for network");


        sockaddr_in sockAddrIn{};
        sockAddrIn.sin_port = htons(ipAddr.port);
        sockAddrIn.sin_family = AF_INET;
        sockAddrIn.sin_addr.s_addr = htonl(
                (ipAddr.octets.parts[0] << 24) +
                (ipAddr.octets.parts[1] << 16) +
                (ipAddr.octets.parts[2] << 8) +
                ipAddr.octets.parts[3]);

        const auto *pSockAddr = reinterpret_cast<sockaddr *> (&sockAddrIn);
        constexpr socklen_t sockAddrLen = sizeof(sockaddr);
        if (const int err = connect(sock, pSockAddr, sockAddrLen); err < 0) {
            const std::string errStr = "network: error connection to server. Error:" + std::to_string(err);
            throw NetworkException(errStr.c_str());
        }

        return sock;
    }

    void sendData(const int sock, const std::vector<uint8_t> &buff) {
        const size_t sendedSize = write(sock, buff.data(), buff.size());
        if (sendedSize <= 0)
            throw NetworkException("Server has closed the connection or another network error.");
        if (sendedSize != buff.size())
            throw NetworkException("Not fully sended data to server");

    }


    int createServerListenSock(const int port) {

        const int servListenSock = socket(AF_INET, SOCK_STREAM, 0);
        constexpr int ERROR_SOCKET = -1;
        if (servListenSock == ERROR_SOCKET)
            throw NetworkException("Error creation server socket");

        constexpr int reuse = 1;
        if (setsockopt(servListenSock, SOL_SOCKET, SO_REUSEADDR,
                       &reuse, sizeof(reuse)) == -1) {
            throw NetworkException("Error setsockopt server socket");
        }

        struct sockaddr_in sockAddr{};
        memset(&sockAddr, 0, sizeof(struct sockaddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = INADDR_ANY;
        sockAddr.sin_port = htons(port);
        if (ERROR_SOCKET == bind(servListenSock, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr)))
            throw NetworkException("Error bind server socket");

        if (ERROR_SOCKET == listen(servListenSock, SOMAXCONN))
            throw NetworkException("Error listen server socket");

        return servListenSock;


    }

    int getClientSockFromListenSock(const int listenSock) {
        sockaddr_in incomeAddr_in{};
        auto *pSockAddr = reinterpret_cast<sockaddr *>(&incomeAddr_in);
        socklen_t sockAddrSize = sizeof(sockaddr);
        const int clientSock = accept(listenSock, pSockAddr, &sockAddrSize);
        if (0 > clientSock)
            throw NetworkException("Error accept and becoming network socket");;
        return clientSock;
    }

    std::vector<uint8_t> readDataFromSocket(const int sock) {
        constexpr uint32_t size = 1024;
        char buffer[size];
        if (const ssize_t readed = read(sock, buffer, size); readed > 0) {
            std::vector<uint8_t> vecBuff(buffer, buffer + readed);
            return vecBuff;
        }
        throw NetworkException("Error read data from  socket");
    }
}
