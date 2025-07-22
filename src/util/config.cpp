#include "config.h"

#include <fstream>

#include "logging.h"

namespace util {

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

