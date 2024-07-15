#include "PROXY_SERVER.h"

ProxiServer::ProxiServer(const int fromPort, const IPAddr &toIpAddr):Server(fromPort),TO_IP_ADDR(toIpAddr) {}

ProxiServer::~ProxiServer() {
    for(auto [fst, snd]:_fromServerToClient)
        delSocketFromEpoll(fst);
}

void ProxiServer::reciveSockData(int clientSock,std::vector<uint8_t >& readedData) {
    parseData(readedData);
    try {
        auto node_iterator = _fromServerToClient.find(clientSock);
        if (node_iterator!=_fromServerToClient.end()){
            const auto iterator =_fromServerToClient.find(clientSock);
            const auto sock = iterator->second;
            network::sendData(sock, readedData);
            return;
        }

        node_iterator =_fromClientToServer.find(clientSock);
        if (node_iterator!=_fromClientToServer.end()){
            network::sendData(node_iterator->second, readedData);
            return;
        }

        auto toServSock= network::conn(TO_IP_ADDR);
        network::sendData(toServSock, readedData);
        _fromClientToServer.insert({clientSock,toServSock});
        _fromServerToClient.insert({toServSock,clientSock});
        addSocketToEpoll(toServSock);
    }catch (NetworkException & netEx) {
        debugLog("NetworkException in ProxiServer::reciveSockData: ",netEx.what());
    }
    catch (std::exception & ex) {
        debugLog("Exception in ProxiServer::reciveSockData: ",ex.what());
    }
}

void ProxiServer::parseData(std::vector<uint8_t> &buff) {
    switch(const auto type =  reinterpret_cast<char *>(buff.data()); *type)
    {
        case 'Q':{
            const auto data = reinterpret_cast<char *>(buff.data() + 5);
            const auto strForm = std::string(data);
            debugLog("Simple Query:",strForm);
            break;}
        case 'X': {
            debugLog("X :TERMINATION");
            break;}
        case 'C': {
            debugLog("C Query: COMMIT:ready for query");
            break;}
        case 'T': {
            const auto * pLength =  reinterpret_cast<uint32_t *>(buff.data() + 1);
            const auto pColumnName = reinterpret_cast<char *>(buff.data() + 7);
            const auto strColumnName = std::string(pColumnName);
            const uint32_t length = __bswap_32(*pLength);

            const auto nextPgField= reinterpret_cast<char *>(buff.data()+length+2);
            pLength =  reinterpret_cast<uint32_t *>(nextPgField + 7);
            const auto pTextInField = reinterpret_cast<char *>(nextPgField + 11);
            const auto strTextInField = std::string(pTextInField,__bswap_32(*pLength));
            debugLog("Row description: Column name:",strColumnName,"  Text in field:",strTextInField);
            break;}

        default:
            debugLog("UnKnown Query");;
    }


}
