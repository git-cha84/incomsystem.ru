#pragma once

#include <array>
#include <mutex>
#include <string>
#include <sstream>
#include <unistd.h>


#ifndef NDEBUG
#    define debugLog logs::Write
#    define debugLogFile logs::fileName = const_cast<char*>
#else
#    define debugLog(messTmp, params...)
#    define debugLogFile(param)
#endif

namespace logs {
    static std::mutex consolM;
    static std::string logFileName = "process_"+ std::to_string(getpid())+".log";
    static const char *fileName =logFileName.c_str();

    template<typename T>
    std::string buildString(T value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<typename T, typename ...Args>
    std::string buildString(T first, Args... args) {
        std::ostringstream oss;
        oss << first << " " << buildString(args...);
        return oss.str();
    }

    template<typename ...Args>
    void Write(Args... args) {
        if (auto debugMess = buildString(args...); !debugMess.empty()) {
            const std::lock_guard<std::mutex> lock(consolM);

            std::array<char, 128> TimeBuffer{0};
            const auto now = std::chrono::system_clock::now();
            const auto now_c = std::chrono::system_clock::to_time_t(now);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch());
            const auto milliseconds = ms.count() % 1000;
            const auto st = std::gmtime(&now_c);
            const auto TimeSize = sprintf(TimeBuffer.data(),
                                         " %d:%d:%d : [%02d:%02d:%02d:%03ld] :",
                                         st->tm_year + 1900,
                                         st->tm_mon + 1,
                                         st->tm_mday,
                                         st->tm_hour - 4,
                                         st->tm_min,
                                         st->tm_sec,
                                         milliseconds);


            if (TimeSize != 0)
                fprintf(stdout, TimeBuffer.data(), 0);
            fprintf(stdout, debugMess.data(), 0);
            fprintf(stdout, "\n");
            FILE *f = nullptr;
            f = fopen(fileName, "a+");
            if (f != nullptr) {
                if (TimeSize != 0)
                    fprintf(f, TimeBuffer.data(), 0);
                fprintf(f, debugMess.data(), 0);
                fprintf(f, "\n");
                fclose(f);
            }
        }
    }
}    // namespace logs
