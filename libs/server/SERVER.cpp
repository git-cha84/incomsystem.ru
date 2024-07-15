#include "SERVER.h"

Server::~Server() {
    delSocketFromEpoll(_listenSocket);
    close(_listenSocket);
}

Server::Server(const int port) {
    initServ(port);
}

void Server::initServ(const int port) {
    try {
        initEpoll();
        _listenSocket = network::createServerListenSock(port);
        addSocketToEpoll(_listenSocket);
    } catch (NetworkException &netEx) {
        debugLog("NetworkException in ProxiServer::reciveSockData: ", netEx.what());
        throw;
    }
}

void Server::inputEvent(const int sock) {
    if (sock == _listenSocket) {
        try {
            const auto clientSocket = network::getClientSockFromListenSock(sock);
            _clientsSocketsSet.insert(clientSocket);
            addSocketToEpoll(clientSocket);
        } catch (NetworkException &netEx) {
            debugLog("NetworkException in Server::inputEvent: ", netEx.what());
        }
        catch (std::exception &ex) {
            debugLog("Exception in Server::inputEvent: ", ex.what());
        }
        return;
    }

    try {
        auto readedData = network::readDataFromSocket(sock);
        reciveSockData(sock, readedData);
    } catch (NetworkException &netEx) {
        debugLog("NetworkException in Server::inputEvent: ", netEx.what());
    }
    catch (std::exception &ex) {
        debugLog("Exception in Server::inputEvent: ", ex.what());
    }
}
