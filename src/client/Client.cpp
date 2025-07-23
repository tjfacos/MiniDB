
#include "Client.h"

#include <cstring>

#include <sodium.h>
#include <unistd.h>

#include "util/logging.h"
#include "util/networking.h"

namespace MiniDB {
    Client::Client(sockaddr_in server_addr, std::string& secret) : secret(secret) {
        const int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            util::error("CLIENT: Failed to create socket");
        }
        conn = new Connection(sock, server_addr, sizeof(server_addr));
    }

    Client::~Client() {
        delete conn;
    };

    void Client::connectToServer()
    {
        const struct sockaddr_in server_addr = conn->getAddr();
        if (connect(conn->getSocket(), reinterpret_cast<const struct sockaddr*>(&server_addr), conn->getAddrLen()) < 0) {
            util::error("CLIENT: Failed to connect to server");
        }

        if (!authenticate()) {
            util::error("CLIENT: Failed to authenticate");
        }
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
        if (util::receiveRaw(conn, nonceS, sizeof(nonceS)) < 0) {
            util::report(conn, "AUTHENTICATE FAILED: Failed to receive server nonce");
            return false;
        }

        // Compute hash
        crypto_auth_hmacsha512(hash, nonceS, sizeof(nonceS), key);

        // Return hash to server
        if (!util::sendRaw(conn, hash, sizeof(hash))) {
            util::report(conn, "AUTHENTICATE FAILED: Could not send HMAC to server");
            return false;
        }

        // Receive Acknowledgement
        util::receiveRaw(conn, ack_msg, sizeof(ack_msg));
        if (sodium_memcmp(ack_msg, ack_success, sizeof(ack_success)) != 0) {
            util::report(conn, "AUTHENTICATE FAILED: Server could not authenticate client. DB_SECRET is wrong!");
            return false;
        }

        /* Client -- AUTHENTICATES --> Server */

        // Generate nonceC
        randombytes_buf(nonceC, sizeof(nonceC));

        // Send to server
        if (!util::sendRaw(conn, nonceC, sizeof(nonceC))) {
            util::report(conn, "AUTHENTICATE FAILED: Could not send client nonce");
            return false;
        }

        // Receive Response from server
        if ( util::receiveRaw(conn, response, sizeof(response)) < 0) {
            util::report(conn, "AUTHENTICATE FAILED: Failed to receive HMAC from server");
            return false;
        }

        // Check response against correct hash
        crypto_auth_hmacsha512(hash, nonceC, sizeof(nonceC), key);
        if (sodium_memcmp(response, hash, sizeof(hash)) != 0) {

            // Send Failure to Server
            util::report(conn, "AUTHENTICATE FAILED: Client HMAC does NOT match server response.");
            if (!util::sendRaw(conn, ack_failure, sizeof(ack_failure)))
                util::report(conn, "Failed to send failure acknowledgement to server");
            return false;

        } else {

            // Send Success Acknowledgement
            if (!util::sendRaw(conn, ack_msg, sizeof(ack_msg))) {
                util::report(conn, "AUTHENTICATE FAILED: Client failed to send acknowledgement.");
                return false;
            }

        }

        /* Key Generation and Secure Clean-up */

        uint8_t session_key_seed[sizeof(nonceS) + sizeof(nonceC)];
        std::copy_n(nonceS, sizeof(nonceS), session_key_seed);
        std::copy_n(nonceC, sizeof(nonceC), session_key_seed + sizeof(nonceC));

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

    void Client::AwaitServerReady() const {

        char msg[6];
        char expected[] = "READY";

        util::receiveRaw(conn, msg, sizeof(msg));

        if (memcmp(msg, expected, sizeof(msg)) == 0) {
            util::report(conn, "Server sent READY Signal, preparing to send traffic...");
        }
    }

    void Client::AwaitServerAcknowledgement() const {

        char msg[9];
        char expected[] = "RECEIVED";

        util::receiveRaw(conn, msg, sizeof(msg));

        if (memcmp(msg, expected, sizeof(msg)) == 0) {
            util::report(conn, "Server sent RECEIVED Signal, traffic delivered...");
        }
    }

    void Client::send_message(std::string msg) const {

        AwaitServerReady();

        char buffer[msg.size() + 1];
        memcpy(buffer, msg.data(), msg.size() + 1);

        util::sendEncrypted(conn, buffer, sizeof(buffer));

        AwaitServerAcknowledgement();

    }

    std::string Client::recv_message() const {
        std::vector<uint8_t>* msg = util::receiveEncrypted(conn);
        return std::string{msg->begin(), msg->end()};
    }

}