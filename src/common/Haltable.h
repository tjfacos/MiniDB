//
// Created by tjfacos on 19/07/25.
//

#ifndef HALTABLE_H
#define HALTABLE_H
#include <atomic>

namespace util {

class Haltable {
    std::atomic<bool>& running;

    public:
        explicit Haltable(std::atomic<bool>& running);
        bool isRunning() const;

};

} // util

#endif //HALTABLE_H
