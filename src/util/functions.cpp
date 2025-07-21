//
// Created by tjfacos on 19/07/25.
//

#include "functions.h"

#include <iostream>
#include <fstream>

namespace util {

    void error(const std::string& msg) {
        perror(msg.c_str());
        exit(1);
    }

    void report(const Connection* conn, const std::string &msg) {
        std::cout << "[" << conn->getEndpoint() << "] " << msg << std::endl;
    }

    DBConfig get_config(const std::string& path) {

        std::ifstream config_file(path);
        if (config_file.fail()) {
            error("Could not open config file at " + path);
        }

        std::string line;
        DBConfig config;

        while (getline(config_file, line)) {

            size_t idx = line.find('=');
            if (idx == std::string::npos) continue;

            std::string key = line.substr(0, idx);
            std::string value = line.substr(idx + 1);

            if (key == "DB_SECRET") {
                config.secret = value;
            }

        }

        return config;
    }

}
