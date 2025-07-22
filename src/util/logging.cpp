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

    void report(const Connection* conn, const std::string &msg, bool client) {
        if (conn) {
            std::cout << "[" << conn->getEndpoint() << "] " << msg << std::endl;
        } else if (client) {
            std::cout << "[CLIENT] " << msg << std::endl;
        } else {
            std::cout << "[SERVER] " << msg << std::endl;
        }
    }


}
