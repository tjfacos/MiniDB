//
// Created by tjfacos on 19/07/25.
//

#include "logging.h"

#include <iostream>
#include <fstream>

namespace util {

    void error(const std::string& msg) {
        perror(msg.c_str());
        exit(1);
    }

    void report(const Connection* conn, const std::string &msg) {
        if (!conn) error("NO CONNECTION IN REPORT");
        else std::cout << "[" << conn->getEndpoint() << "] " << msg << std::endl;
    }


}
