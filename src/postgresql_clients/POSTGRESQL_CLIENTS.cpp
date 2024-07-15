#include <iostream>
#include <pqxx/pqxx>
#include <random>
#include <string>
#include <thread>

#include "global_config.h"
#include "NETWORK.h"

std::string genRandString() {
    const uint32_t kMaxRandInt = 50;
    const uint32_t kMixRandInt = 10;
    uint32_t randInt = kMixRandInt + (std::rand() % kMaxRandInt);
    std::random_device randDev;
    std::mt19937 gen(randDev());
    const unsigned char kMinChar = 'A';
    const unsigned char kMsxChar = 'Z';
    std::uniform_int_distribution<unsigned char> dist(kMinChar, kMsxChar);
    std::stringstream ss;
    while ((randInt--) > 0)
        ss << dist(gen);
    return ss.str();
}


void sqlClient(int id) {
    std::string connStr = "dbname = postgres user = postgres password = postgres \
      hostaddr = 127.0.0.1 port = " + std::to_string(proxiPort);
    pqxx::connection conn(connStr.c_str());
    if (conn.is_open()) {
        // Создание базы данных
        pqxx::work txn(conn);

        auto startTime = std::chrono::steady_clock::now();
        while (true) {
            std::string randStr = genRandString();
            std::string insertStr = "INSERT INTO " + tableName + " (rand_str) VALUES ('" + randStr + "')";

            txn.exec(insertStr);

            std::string selectStr = "SELECT rand_str FROM " + tableName + " WHERE rand_str = '" + randStr + "'";
            auto res = txn.exec(selectStr);

            for (auto row: res) {
                if (randStr == row["rand_str"].c_str()) {
                    // debugLog("correct id:",id);
                } else {
                    debugLog("ERROR!");
                    throw 1;
                }
            }
            auto currentTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedTime = currentTime - startTime;
            if (elapsedTime.count() >= RUNNING_TIME) {
                break;
            }
        }
        txn.commit();
    }
    conn.close();
}


int main() {
    std::cout << "Clients Begin:" << std::endl;
    std::vector<std::thread> bufTh;
    for (int i = 0; i < MAX_TH; i++) {
        auto th = std::thread(&sqlClient, i);
        bufTh.push_back(std::move(th));
    }

    for (auto &i: bufTh) {
        if (i.joinable())
            i.join();
    }

    std::cout << "Clients end:" << std::endl;
    return 0;
}
