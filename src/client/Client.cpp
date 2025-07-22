
#include "Client.h"

#include <cstring>

#include <sodium.h>

#include "util/functions.h"
#include "util/Haltable.h"

namespace MiniDB {
    Client::Client(sockaddr_in server_addr, std::string& secret) : server_addr(server_addr), secret(secret) {}

    Client::~Client() = default;

    void Client::run()
    {
        socket = socket(AF_INET, SOCK_STREAM, 0);
        if (socket < 0) {
            util::error("CLIENT: Failed to create socket");
        }

        if (connect(socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            util::error("CLIENT: Failed to connect to server");
        }

        if (!authenticate()) {
            util::error("CLIENT: Failed to authenticate");
        }

        char msg[] = "Hello, World!";
        util::sendBytes(socket, msg, strlen(msg));

        close(socket);
    }

    bool Client::authenticate()
    {

        uint8_t nonceC[crypto_secretbox_NONCEBYTES];
        uint8_t nonceS[crypto_secretbox_NONCEBYTES];

        uint8_t response[crypto_auth_hmacsha512_BYTES];
        uint8_t hash[crypto_auth_hmacsha512_BYTES];
        uint8_t key[crypto_auth_hmacsha512_KEYBYTES];

        char ack_msg[] = "OK";

        // Securely zero all buffers
        sodium_memzero(nonceC, sizeof(nonceC));
        sodium_memzero(nonceS, sizeof(nonceS));
        sodium_memzero(response, sizeof(response));
        sodium_memzero(hash, sizeof(hash));
        sodium_memzero(key, sizeof(key));

        // Transfer db secret to key buffer
        memcpy(key, secret.data(), secret.size());

        /* Server -- AUTHENTICATES --> Client */

        // Receive nonceS from server
        if (util::receiveBytes(socket, nonceS, sizeof(nonceS)) < 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Failed to receive server nonce", true);
            return false;
        }

        // Compute hash
        crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);

        // Return hash to server
        if (!util::sendBytes(socket, hash, sizeof(hash))) {
            util::report(nullptr, "AUTHENTICATE FAILED: Could not send HMAC to server", true);
            return false;
        }

        // Receive OK
        util::receiveBytes(socket, ack_msg, sizeof(ack_msg));


        /* Client -- AUTHENTICATES --> Server */

        // Generate nonceC
        randombytes_buf(nonceC, sizeof(nonceC));

        // Send to server
        if (!util::sendBytes(socket, nonceC, sizeof(nonceC))) {
            util::report(nullptr, "AUTHENTICATE FAILED: Could not send client nonce", true);
            return false;
        }

        // Receive Response from server
        if ( util::receiveBytes(socket, response, sizeof(response)) < 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Failed to receive HMAC from server", true);
            return false;
        }

        // Check response against correct hash
        crypto_auth_hmacsha512(hash, nonceC, sizeof(nonceC), key);

        if (sodium_memcmp(response, hash, sizeof(hash)) != 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Client HMAC does NOT match server response.", true);
            return false;
        }

        if (!util::sendBytes(socket, ack_msg, sizeof(ack_msg))) {
            util::report(nullptr, "AUTHENTICATE FAILED: Client failed to send acknowledgement.", true);
            return false;
        }

        return true;
    }

    // ssize_t Client::send_message(void* buffer, size_t length)
    // {
    //     send(socket, buffer, length, 0);
    // }
    //
    // uint8_t* Client::recv_message()
    // {
    //     // TODO Implement Message Protocol (see NOTES.md)
    //     return nullptr;
    // }
} // MiniDB