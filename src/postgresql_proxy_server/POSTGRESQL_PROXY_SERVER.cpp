#include <iostream>

#include "global_config.h"
#include "PROXY_SERVER.h"


int main() {
    IPAddr ipAddr{};
    ipAddr.octets = {127, 0, 0, 1};
    ipAddr.port = postgresPort;
    ProxiServer proxiServer(proxiPort, ipAddr);
    std::cout << "Start Proxy Server:" << std::endl;
    std::string str;
    do {
        str.clear();
        std::cin >> str;
    }while (str!="EXIT");
    std::cout << "End Proxy Server:" << std::endl;
    return 0;
}
