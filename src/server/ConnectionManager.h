//
// Created by tjfacos on 19/07/25.
//

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <mutex>
#include <set>

#include "common/Connection.h"


class ConnectionManager {
    std::set<Connection*> connections;
    std::set<std::thread> threads;
    std::mutex mut;

    public:
        ConnectionManager();
        ~ConnectionManager();

        void add(Connection* conn);
        void del(Connection* conn);
        bool empty() const;
};



#endif //CONNECTIONMANAGER_H
