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

    void report(const Connection* conn, const std::string &msg, bool client) {
        if (conn) {
            std::cout << "[" << conn->getEndpoint() << "] " << msg << std::endl;
        } else if (client) {
            std::cout << "[CLIENT] " << msg << std::endl;
        } else {
            std::cout << "[SERVER] " << msg << std::endl;
        }
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

    bool sendBytes(int sock, void* buffer, size_t len) {

        size_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {

            auto sent_bytes = send(sock, ptr + offset, len - offset, 0);

            if (sent_bytes == -1) {
                return false;
            }

            offset += sent_bytes;

        } while (offset < len);

        return true;
    }

    ssize_t receiveBytes(int sock, void *buffer, size_t len) {

        ssize_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {
            auto recv_bytes = recv(sock, ptr + offset, len - offset, 0);
            if (recv_bytes <= 0) return -1;
            offset += recv_bytes;
        } while (offset < len);

        return offset;
    }

}
