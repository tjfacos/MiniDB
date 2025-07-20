//
// Created by tjfacos on 19/07/25.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <bits/std_thread.h>
#include <netinet/in.h>

class Connection {

    int socket;
    sockaddr_in addr;
    size_t addrlen;

    public:
        Connection(int socket, sockaddr_in addr, socklen_t addrlen);
        ~Connection();

        int         getSocket()     const;
        sockaddr_in getAddr()       const;
        size_t      getAddrLen()    const;

        std::string getIP()         const;
        int         getPort()       const;
        std::string getEndpoint()   const;

};


#endif //CONNECTION_H
