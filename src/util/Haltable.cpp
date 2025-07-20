//
// Created by tjfacos on 19/07/25.
//

#include "Haltable.h"

namespace util {
    Haltable::Haltable(std::atomic<bool>& running) : running(running) {}

    bool Haltable::isRunning() const {
        return this->running;
    };
} // util