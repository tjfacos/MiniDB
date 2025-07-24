//
// Created by tjfacos on 19/07/25.
//

#ifndef UTIL_H
#define UTIL_H
#include <string>

#include "Connection.h"

namespace util {
    void error(const std::string& msg);
    void report(const Connection* conn, const std::string &msg);
}

#endif //UTIL_H
