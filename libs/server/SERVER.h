#ifndef SERVER_H
#define SERVER_H


#include "BASE_EPOLL.h"
#include "NETWORK.h"

class Server: public BaseEpoll {

public:
    Server() =delete;
    Server(const Server&) =delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

    explicit Server( int port);
     ~Server() override;

protected:

   virtual void reciveSockData(int sock,std::vector<uint8_t >& buff)=0;

private:
    std::unordered_set<int> _clientsSocketsSet{};
    int _listenSocket{};
    void initServ(int port);
    void inputEvent(int sock)  override;

};

#endif
