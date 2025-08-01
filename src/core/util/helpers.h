//
// Created by root on 8/1/25.
//

#ifndef HELPERS_H
#define HELPERS_H
#include <unordered_map>

namespace util {

    template <typename K, typename V>
    auto invertMap(const std::unordered_map<K, V>& map);

}

#endif //HELPERS_H
