#include "Server.h"

#include <cstring>
#include <iostream>
#include <strings.h>
#include <thread>
#include <fcntl.h>

#include <sodium.h>

#include "util/logging.h"
#include "../common/messaging/networking.h"

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

    char READY_ACK[]    = "READY";
    char RECEIVED_ACK[] = "RECEIVED";

    util::report(conn, "NEW CONNECTION! STARTING HANDSHAKE...");

    if (!authenticate(conn)) {
        util::report(conn, "AUTHENTICATION FAILED, DISCONNECTING...");
        return;
    }

    util::report(conn, "AUTHENTICATION SUCCESSFUL! CONNECTION ESTABLISHED.");

    char buffer[1024];
    int sock = conn->getSocket();

    while (this->isRunning()) {

        // Send Ready Signal
        util::sendRaw(conn, READY_ACK, sizeof(READY_ACK));
        util::report(conn, "Ready to receive traffic...");

        // Get message from client
        std::vector<uint8_t>* msg = util::receiveEncrypted(conn);

        // Check for a null message
        if (msg == nullptr && util::net_errno == MIMP_DISCONNECT) {
            util::report(conn, "CONNECTION CLOSED GRACEFULLY.");
            return;
        }

        if (msg == nullptr) {
            util::error("Failed to receive message.");
            return;
        }

        // Send acknowledge signal
        util::sendRaw(conn, RECEIVED_ACK, sizeof(RECEIVED_ACK));
        util::report(conn, "Traffic Received and Acknowledged...");

        std::cout << "[" << conn->getEndpoint() << "] " << "MESSAGE: " << reinterpret_cast<char *>(msg->data()) << std::endl;
    }
}

bool Server::authenticate(const Connection *conn) const {

    uint8_t nonceS[crypto_secretbox_NONCEBYTES];
    uint8_t nonceC[crypto_secretbox_NONCEBYTES];

    uint8_t response[crypto_auth_hmacsha512_BYTES];
    uint8_t hash[crypto_auth_hmacsha512_BYTES];
    uint8_t key[crypto_auth_hmacsha512_KEYBYTES];

    char ack_msg[3];

    char ack_success[] = "OK";
    char ack_failure[] = "NO";

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
    if (!util::sendRaw(conn, nonceS, sizeof(nonceS))) {
        util::report(conn, "AUTHENTICATE FAILED: Could not send server nonce");
        return false;
    }

    // Receive Response from client
    if (util::receiveRaw(conn, response, sizeof(response)) < 0) {
        util::report(conn, "AUTHENTICATE FAILED: Failed to receive HMAC from client");
        return false;
    }

    // Check response against correct hash
    crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);
    if (sodium_memcmp(response, hash, sizeof(hash)) != 0) {

        // Send Failure Acknowledgement to client
        util::report(conn, "AUTHENTICATE FAILED: Hash of server nonce does NOT match client response.");
        if (!util::sendRaw(conn, ack_failure, sizeof(ack_failure)))
            util::report(conn, "Failed to send failure acknowledgement to client");
        return false;

    } else {

        // Send Success Acknowledgement
        if (!util::sendRaw(conn, ack_success, sizeof(ack_success))) {
            util::report(conn, "AUTHENTICATE FAILED: Server failed to send acknowledgement.");
            return false;
        }

    }

    /* Client -- AUTHENTICATES --> Server */

    // Receive nonceC from client
    if (util::receiveRaw(conn, nonceC, sizeof(nonceC)) < 0) {
        util::report(conn, "AUTHENTICATE FAILED: Failed to receive client nonce");
        return false;
    }

    // Compute hash
    crypto_auth_hmacsha512(hash, nonceC, sizeof(nonceC), key);

    // Return hash to client
    if (!util::sendRaw(conn, hash, sizeof(hash))) {
        util::report(conn, "AUTHENTICATE FAILED: Could not send HMAC to client");
        return false;
    }

    // Receive Acknowledgement
    util::receiveRaw(conn, ack_msg, sizeof(ack_msg));
    if (sodium_memcmp(ack_msg, ack_success, sizeof(ack_success)) != 0) {
        util::report(conn, "AUTHENTICATE FAILED: Client could not authenticate server");
        return false;
    }


    /* Key Generation and Secure Clean-up */

    uint8_t session_key_seed[sizeof(nonceS) + sizeof(nonceC)];
    std::memcpy(session_key_seed, nonceS, sizeof(nonceS));
    std::memcpy(session_key_seed + sizeof(nonceS), nonceC, sizeof(nonceC));

    crypto_auth_hmacsha512(conn->sessionKey(), session_key_seed, sizeof(session_key_seed), key);

    sodium_memzero(nonceS           , sizeof(nonceS             ));
    sodium_memzero(nonceC           , sizeof(nonceC             ));
    sodium_memzero(response         , sizeof(response           ));
    sodium_memzero(hash             , sizeof(hash               ));
    sodium_memzero(key              , sizeof(key                ));
    sodium_memzero(ack_msg          , sizeof(ack_msg            ));
    sodium_memzero(session_key_seed , sizeof(session_key_seed   ));

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
