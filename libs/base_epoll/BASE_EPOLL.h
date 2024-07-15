#ifndef EPOOL_WRAPPER_H
#define EPOOL_WRAPPER_H

#include <unistd.h>
#include <sys/epoll.h>

#include <atomic>
#include <thread>
#include <unordered_set>
#include <vector>

class BaseEpoll {
 public:
    BaseEpoll(const BaseEpoll &) = delete;

    BaseEpoll(BaseEpoll &&) = delete;

    BaseEpoll &operator=(const BaseEpoll &) = delete;

    BaseEpoll &operator=(BaseEpoll &&) = delete;

    BaseEpoll() = default;

    virtual ~BaseEpoll();

 protected:
    bool initEpoll();

    bool addSocketToEpoll(int sock);

    void delSocketFromEpoll(int sock) const;

    virtual void inputEvent(int sock) = 0;

 private:
    # define mem_ord  std::memory_order_seq_cst
    const int MAX_EVENTS = 1024;
    const int TIMEOUT = 1;
    int _epfd{-1};
    std::atomic<bool> _isEpollWait{false};
    std::thread _waitReciveDataTh;
    std::unordered_set<int> _setSockets{};

    void waitReciveData();

    void closeAllSockets();
};


#endif //EPOOL_WRAPPER_H
