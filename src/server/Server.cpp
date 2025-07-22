#include "Server.h"

#include <cstring>
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

    util::report(conn, "NEW CONNECTION");
    util::report(conn, "STARTING HANDSHAKE...");

    if (!authenticate(conn)) {
        util::report(conn, "AUTHENTICATION FAILED, DISCONNECTING...");
        return;
    }

    util::report(conn, "AUTHENTICATION SUCCESSFUL! CONNECTION ESTABLISHED.");

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

bool Server::authenticate(const Connection *conn) const {

    uint8_t nonceS[crypto_secretbox_NONCEBYTES];
    uint8_t nonceC[crypto_secretbox_NONCEBYTES];

    uint8_t response[crypto_auth_hmacsha512_BYTES];
    uint8_t hash[crypto_auth_hmacsha512_BYTES];
    uint8_t key[crypto_auth_hmacsha512_KEYBYTES];

    char ack_msg[] = "OK";

    // Securely zero all buffers
    sodium_memzero(nonceS, sizeof(nonceS));
    sodium_memzero(nonceC, sizeof(nonceC));
    sodium_memzero(response, sizeof(response));
    sodium_memzero(hash, sizeof(hash));
    sodium_memzero(key, sizeof(key));

    // Transfer db secret to key buffer
    memcpy(key, secret, sizeof(secret));

    /* Server -- AUTHENTICATES --> Client */

    // Generate nonceS
    randombytes_buf(nonceS, sizeof(nonceS));

    // Send to client
    if (!util::sendBytes(conn->getSocket(), nonceS, sizeof(nonceS))) {
        util::report(conn, "AUTHENTICATE FAILED: Could not send server nonce");
        return false;
    }

    // Receive Response from client
    if (util::receiveBytes(conn->getSocket(), response, sizeof(response)) < 0) {
        util::report(conn, "AUTHENTICATE FAILED: Failed to receive HMAC from client");
        return false;
    }

    // Check response against correct hash
    crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);

    if (sodium_memcmp(response, hash, sizeof(hash)) != 0) {
        util::report(conn, "AUTHENTICATE FAILED: Hash of server nonce does NOT match client response.");
        return false;
    }

    if (!util::sendBytes(conn->getSocket(), ack_msg, sizeof(ack_msg))) {
        util::report(conn, "AUTHENTICATE FAILED: Server failed to send acknowledgement.");
        return false;
    }


    /* Client -- AUTHENTICATES --> Server */

    // Receive nonceC from client
    if (util::receiveBytes(conn->getSocket(), nonceC, sizeof(nonceC)) < 0) {
        util::report(conn, "AUTHENTICATE FAILED: Failed to receive client nonce");
        return false;
    }

    // Compute hash
    crypto_auth_hmacsha512(hash, nonceC, sizeof(nonceC), key);

    // Return hash to client
    if (!util::sendBytes(conn->getSocket(), hash, sizeof(hash))) {
        util::report(conn, "AUTHENTICATE FAILED: Could not send HMAC to client");
        return false;
    }

    // Receive OK
    util::receiveBytes(conn->getSocket(), ack_msg, sizeof(ack_msg));

    return true;
}

Server::Server(std::atomic<bool>& flag) : Haltable(flag), connection_manager(ConnectionManager()), secret{} {}

Server::~Server() {
    // Prevent destruction while connections haven't fully closed
    do { std::this_thread::sleep_for(std::chrono::milliseconds(10)); } while (!connection_manager.empty());
};

void Server::setSecret(const std::string &secret) {
    memcpy(this->secret, secret.data(), crypto_auth_hmacsha512_KEYBYTES);
}

void Server::run(const int port) {
    int socket = buildSocket(port);

    // TODO Remove
    std::cout << "Server Secret :: " << secret << " (l: " << sizeof(secret) << ")" << std::endl;

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
