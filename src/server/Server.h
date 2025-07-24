#ifndef SERVER_H
#define SERVER_H

#include <sodium.h>

#include "ConnectionManager.h"
#include "common/Haltable.h"

class Server : public util::Haltable {

    ConnectionManager connection_manager;
    char secret[crypto_auth_hmacsha512_KEYBYTES];

    static int buildSocket(int port);

    void handleConnection(const Connection *conn);

    bool authenticate(const Connection *conn) const;

    public:
        explicit Server(std::atomic<bool>& flag);
        ~Server();
        void setSecret(const std::string &secret);
        void run(int port);

};



#endif //SERVER_H
