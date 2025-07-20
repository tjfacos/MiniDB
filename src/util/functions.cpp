//
// Created by tjfacos on 19/07/25.
//

#include "functions.h"

#include <iostream>

namespace util {
    void error(const std::string& msg) {
        perror(msg.c_str());
        exit(1);
    }

    void report(const Connection* conn, const std::string &msg) {
        std::cout << "[" << conn->getEndpoint() << "] " << msg << std::endl;
    }
}
