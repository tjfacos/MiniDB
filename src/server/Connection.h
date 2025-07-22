//
// Created by tjfacos on 19/07/25.
//

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <bits/std_thread.h>
#include <netinet/in.h>
#include <sodium.h>

class Connection {

    int socket;
    sockaddr_in addr;
    size_t addrlen;

    uint8_t session_key[crypto_auth_hmacsha512_BYTES];

    public:
        Connection(int socket, sockaddr_in addr, socklen_t addrlen);
        ~Connection();

        int         getSocket()     const;
        sockaddr_in getAddr()       const;
        size_t      getAddrLen()    const;

        std::string getIP()         const;
        int         getPort()       const;
        std::string getEndpoint()   const;

        uint8_t*    getSessionKey() const;

};


#endif //CONNECTION_H
