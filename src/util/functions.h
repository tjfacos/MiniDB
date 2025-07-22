//
// Created by tjfacos on 19/07/25.
//

#ifndef UTIL_H
#define UTIL_H
#include <string>

#include "server/Connection.h"

namespace util {

    struct DBConfig {
        std::string secret;
    };

    void error(const std::string& msg);
    void report(const Connection* conn = nullptr, const std::string &msg = "", bool client = false);

    DBConfig get_config(const std::string& path);

    bool    sendBytes(int sock, void* buffer, size_t len);
    ssize_t receiveBytes(int sock, void *buffer, size_t len);

}

#endif //UTIL_H
