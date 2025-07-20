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
    void report(const Connection* conn, const std::string& msg);

    DBConfig get_config(const std::string& path);
}

#endif //UTIL_H
