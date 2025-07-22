//
// Created by root on 7/22/25.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <string>


namespace util {

    struct DBConfig {
        std::string secret;
    };

    DBConfig get_config(const std::string& path);
}



#endif //CONFIG_H
