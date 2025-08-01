//
// Created by root on 8/1/25.
//

#include <unordered_map>

namespace util {
    template <typename K, typename V>
    auto invertMap(const std::unordered_map<K, V>& map) {
        std::unordered_map<V, K> m;
        for (const auto& pair : map) {
            m[pair.second] = pair.first;
        }
        return m;
    }
}
