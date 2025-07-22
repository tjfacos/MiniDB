#ifndef SERVER_H
#define SERVER_H

#include <sodium.h>

#include "ConnectionManager.h"
#include "util/Haltable.h"

class Server : public util::Haltable {

    ConnectionManager connection_manager;
    char secret[crypto_auth_hmacsha512_KEYBYTES];

    static int buildSocket(int port);

    void handleConnection(const Connection *conn);

    static bool transmit(const Connection *conn, void *buffer, size_t len);
    static ssize_t receiveBytes(const Connection *conn, void *buffer, size_t len);

    bool authenticate(const Connection *conn) const;

    public:
        Server(std::atomic<bool>& flag);
        ~Server();
        void setSecret(const std::string &secret);
        void run(int port);

};



#endif //SERVER_H
