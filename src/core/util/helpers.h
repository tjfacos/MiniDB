#ifndef HELPERS_H
#define HELPERS_H
#include <unordered_map>

namespace util {

    template <typename K, typename V>
    std::unordered_map<V, K> invertMap(const std::unordered_map<K, V>& map) {
        std::unordered_map<V, K> m;
        for (const auto& pair : map) {
            m[pair.second] = pair.first;
        }
        return m;
    }

}

#endif //HELPERS_H