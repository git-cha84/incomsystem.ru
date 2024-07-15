
#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H
#include <string>

inline int postgresPort = 5432;
inline int proxiPort = 6633;
inline std::string tableName = "TEST_TABLE";
constexpr int RUNNING_TIME = 300;
constexpr int MAX_TH = 50;

#endif //GLOBAL_CONFIG_H
