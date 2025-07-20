#include "Server.h"

#include <iostream>
#include <strings.h>
#include <thread>
#include <fcntl.h>

#include <sodium.h>

#include "util/functions.h"

int Server::buildSocket(int port) {

    // Establish socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        util::error("Failed to initialise socket.");

    // Create address
    sockaddr_in addr{};
    bzero((char*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind port to address
    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
        util::error("Failed to bind socket to address.");

    return sockfd;
}

void Server::handleConnection(const Connection *conn) {
    // TODO : Capture Messages and Parse

    util::report(conn, "NEW CONNECTION START");

    char buffer[1024];
    int sock = conn->getSocket();

    while (this->isRunning()) {

        bzero(buffer, 1024);
        auto num_bytes = recv(sock, buffer, sizeof(buffer), MSG_DONTWAIT);

        if (num_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (num_bytes == 0) {
            util::report(conn, "CONNECTION CLOSED GRACEFULLY.");
            return;
        }

        if (num_bytes < 0) {
            util::error("Failed to receive message.");
        }

        std::cout << "[" << conn->getEndpoint() << "] " << "MESSAGE: " << buffer << std::endl;
    }
}

bool Server::transmit(const Connection *conn, void* buffer, size_t len) {

    size_t offset = 0;

    do {

        auto sent_bytes = send(conn->getSocket(), buffer + offset, len - offset, 0);

        if (sent_bytes == -1) {
            return false;
        }

        offset += sent_bytes;

    } while (offset < len);

    return true;
}

ssize_t Server::receiveBytes(const Connection *conn, void *buffer, size_t len) {

    size_t offset = 0;

    do {
        auto recv_bytes = recv(conn->getSocket(), buffer + offset, len - offset, 0);
        if (recv_bytes <= 0) return -1;
        offset += recv_bytes;
    } while (offset < len);

    return offset;
}

bool Server::authenticate(const Connection *conn) {

    uint8_t nonceS[crypto_secretbox_NONCEBYTES];
    uint8_t nonceC[crypto_secretbox_NONCEBYTES];

    uint8_t response[crypto_auth_hmacsha512_BYTES];
    uint8_t hash[crypto_auth_hmacsha512_BYTES];
    uint8_t key[crypto_auth_hmacsha512_KEYBYTES];

    crypto_auth_hmacsha512_state state;

    // Generate nonceS
    randombytes_buf(nonceS, sizeof(nonceS));

    // Send to client
    if (!transmit(conn, nonceS, sizeof(nonceS))) {
        util::report(conn, "AUTHENTICATE FAILED: Could not sent server nonce");
        return false;
    }

    // Receive Response from client
    if ( receiveBytes(conn, response, sizeof(response)) < 0) {
        util::report(conn, "AUTHENTICATE FAILED: Failed to receive HMAC from client");
        return false;
    }

    // Check response against correct hash
    crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);




}

Server::Server(std::atomic<bool>& flag) : Haltable(flag), connection_manager(ConnectionManager()) {}

Server::~Server() {
    // Prevent destruction while connections haven't fully closed'
    do { std::this_thread::sleep_for(std::chrono::milliseconds(10)); } while (!connection_manager.empty());
    std::cout << "ALL CONNECTIONS CLOSED. SHUTTING DOWN SERVER..." << std::endl;
};

void Server::run(const int port) {
    int socket = buildSocket(port);

    // Make listen socket non-blocking
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);

    listen(socket, 5);
    std::cout << "Server listening on port " << port << std::endl;

    while (this->isRunning()) {

        sockaddr_in client_addr;
        socklen_t cli_addr_len = sizeof(client_addr);

        int conn = accept(socket, reinterpret_cast<struct sockaddr*>(&client_addr), &cli_addr_len);

        if (conn == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (conn < 0) {
            util::error("Failed to accept connection");
        }

        auto connection = new Connection(conn, client_addr, cli_addr_len);


        std::thread t([this, connection]() {
            connection_manager.add(connection);
            handleConnection(connection);
            connection_manager.del(connection);
            delete connection;
        });

        t.detach();
    }

}
