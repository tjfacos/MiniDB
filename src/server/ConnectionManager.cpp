//
// Created by tjfacos on 19/07/25.
//

#include "ConnectionManager.h"

ConnectionManager::ConnectionManager() = default;

ConnectionManager::~ConnectionManager() = default;

void ConnectionManager::add(Connection *conn) {
    std::unique_lock<std::mutex> lock(mut);
    connections.insert(conn);
}

void ConnectionManager::del(Connection *conn) {
    std::unique_lock<std::mutex> lock(mut);
    connections.erase(conn);
}

bool ConnectionManager::empty() const {
    return connections.empty();
}

