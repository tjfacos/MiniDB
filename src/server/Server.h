#ifndef SERVER_H
#define SERVER_H

#include "ConnectionManager.h"
#include "util/Haltable.h"

class Server : public util::Haltable {
    ConnectionManager connection_manager;

    static int buildSocket(int port);
    void handleConnection(const Connection *conn);

    public:
        Server(std::atomic<bool>& flag);
        ~Server();
        void run(int port);
};



#endif //SERVER_H
