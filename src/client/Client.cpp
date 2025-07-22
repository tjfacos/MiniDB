
#include "Client.h"

#include <cstring>

#include <sodium.h>
#include <unistd.h>

#include "util/logging.h"
#include "util/networking.h"

namespace MiniDB {
    Client::Client(sockaddr_in server_addr, std::string& secret) : server_addr(server_addr), secret(secret) {}

    Client::~Client() = default;

    void Client::test()
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock < 0) {
            util::error("CLIENT: Failed to create socket");
        }

        if (connect(sock, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            util::error("CLIENT: Failed to connect to server");
        }

        if (!authenticate()) {
            util::error("CLIENT: Failed to authenticate");
        }

        char msg[] = "Hello, World!";
        util::sendBytes(sock, msg, strlen(msg));

        close(sock);
    }

    bool Client::authenticate()
    {

        uint8_t nonceC[crypto_secretbox_NONCEBYTES];
        uint8_t nonceS[crypto_secretbox_NONCEBYTES];

        uint8_t response[crypto_auth_hmacsha512_BYTES];
        uint8_t hash[crypto_auth_hmacsha512_BYTES];
        uint8_t key[crypto_auth_hmacsha512_KEYBYTES];

        char ack_msg[3];
        char ack_success[] = "OK";
        char ack_failure[] = "NO";

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
        if (util::receiveBytes(sock, nonceS, sizeof(nonceS)) < 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Failed to receive server nonce", true);
            return false;
        }

        // Compute hash
        crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);

        // Return hash to server
        if (!util::sendBytes(sock, hash, sizeof(hash))) {
            util::report(nullptr, "AUTHENTICATE FAILED: Could not send HMAC to server", true);
            return false;
        }

        // Receive Acknowledgement
        util::receiveBytes(sock, ack_msg, sizeof(ack_msg));
        if (sodium_memcmp(ack_msg, ack_success, sizeof(ack_success)) != 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Server could not authenticate client. DB_SECRET is wrong!", true);
            return false;
        }

        /* Client -- AUTHENTICATES --> Server */

        // Generate nonceC
        randombytes_buf(nonceC, sizeof(nonceC));

        // Send to server
        if (!util::sendBytes(sock, nonceC, sizeof(nonceC))) {
            util::report(nullptr, "AUTHENTICATE FAILED: Could not send client nonce", true);
            return false;
        }

        // Receive Response from server
        if ( util::receiveBytes(sock, response, sizeof(response)) < 0) {
            util::report(nullptr, "AUTHENTICATE FAILED: Failed to receive HMAC from server", true);
            return false;
        }

        // Check response against correct hash
        crypto_auth_hmacsha512(hash, nonceC, sizeof(nonceC), key);
        if (sodium_memcmp(response, hash, sizeof(hash)) != 0) {

            // Send Failure to Server
            util::report(nullptr, "AUTHENTICATE FAILED: Client HMAC does NOT match server response.", true);
            if (!util::sendBytes(sock, ack_failure, sizeof(ack_failure)))
                util::report(nullptr, "Failed to send failure acknowledgement to server", true);
            return false;

        } else {

            // Send Success Acknowledgement
            if (!util::sendBytes(sock, ack_msg, sizeof(ack_msg))) {
                util::report(nullptr, "AUTHENTICATE FAILED: Client failed to send acknowledgement.", true);
                return false;
            }

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