#ifndef PROXI_SERVER_H
#define PROXI_SERVER_H

#include <unordered_map>

#include "SERVER.h"

class ProxiServer final : public Server {
public:
    ProxiServer() = delete;
    ProxiServer(const ProxiServer&) = delete;
    ProxiServer(ProxiServer&&) = delete;
    ProxiServer& operator=(const ProxiServer&) = delete;
    ProxiServer& operator=(ProxiServer&&) = delete;

    ProxiServer(int fromPort,const IPAddr &toIpAddr);
    ~ProxiServer() override;

private:
    const IPAddr TO_IP_ADDR;
    std::unordered_map<int,int> _fromServerToClient{};
    std::unordered_map<int,int> _fromClientToServer{};

    void reciveSockData(int clientSock,std::vector<uint8_t >& readedData) override ;
    static void parseData(std::vector<uint8_t >& buff);
};

#endif
