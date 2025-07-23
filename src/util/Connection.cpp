#include "Connection.h"

#include <unistd.h>
#include <arpa/inet.h>

#include "util/logging.h"

Connection::Connection(int socket, sockaddr_in addr, socklen_t addrlen)
    : socket(socket), addr(addr), addrlen(addrlen), session_key{} {}

Connection::~Connection() {
    util::report(this, "Closing...");
    close(socket);
}

int Connection::getSocket() const {
    return socket;
}

sockaddr_in Connection::getAddr() const {
    return addr;
}

size_t Connection::getAddrLen() const {
    return addrlen;
}

std::string Connection::getIP() const {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(this->addr.sin_addr), clientIP, INET_ADDRSTRLEN);
    return clientIP;
}

int Connection::getPort() const {
    return ntohs(this->addr.sin_port);
}

std::string Connection::getEndpoint() const {
    return this->getIP() + ":" + std::to_string(this->getPort());
}

uint8_t* Connection::sessionKey() const {
    return const_cast<uint8_t *>(session_key);
}