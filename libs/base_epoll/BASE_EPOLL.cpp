#include "BASE_EPOLL.h"

BaseEpoll::~BaseEpoll() {
    closeAllSockets();
    _isEpollWait.store(false, mem_ord);
    if (_waitReciveDataTh.joinable()) {
        try {
            _waitReciveDataTh.join();
        } catch (...) {
        }
    }
    if (_epfd != -1) {
        close(_epfd);
    }
}

void BaseEpoll::closeAllSockets() {
    for (auto it = _setSockets.begin(); it != _setSockets.end();) {
        delSocketFromEpoll(*it);
        it = _setSockets.erase(it);
    }
}

void BaseEpoll::waitReciveData() {
    std::vector<epoll_event> evlist;
    evlist.reserve(MAX_EVENTS);

    while (_isEpollWait.load(mem_ord)) {
        const int nfds = epoll_wait(_epfd, evlist.data(), MAX_EVENTS, TIMEOUT);
        for (int i = 0; (_isEpollWait.load(mem_ord)) && (i < nfds); i++) {
            const int sock = evlist[i].data.fd;
            inputEvent(sock);
        }
    }
}

bool BaseEpoll::addSocketToEpoll(const int sock) {
    if (_epfd < 0) {
        return false;
    }

    if (_setSockets.find(sock) == _setSockets.end()) {
        epoll_event event{};
        event.events = EPOLLIN;
        event.data.fd = sock;
        if (const int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &event); ret < 0) {
            return false;
        }
    }
    _setSockets.insert(sock);
    return true;
}

bool BaseEpoll::initEpoll() {
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) {
        return false;
    }
    try {
        _isEpollWait.store(true, mem_ord);
        _waitReciveDataTh = std::thread(&BaseEpoll::waitReciveData, this);
    } catch (...) {
        _isEpollWait.store(false, mem_ord);
        close(_epfd);
        _epfd = -1;
        return false;
    }
    return true;
}

void BaseEpoll::delSocketFromEpoll(const int sock) const {
    epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
    close(sock);
}
