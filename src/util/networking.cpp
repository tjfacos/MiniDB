//
// Created by root on 7/22/25.
//

#include "networking.h"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sodium.h>
#include <thread>
#include <vector>
#include <vector>

#include "logging.h"

#define MIMP_HEADER_BYTES   (4 + crypto_secretbox_NONCEBYTES)
#define MIMP_MAGIC_BYTE     0xDB
#define MIMP_VERSION        1

#define DEFAULT_BUFFER_SIZE 4096

namespace util {

    bool sendRaw(const Connection *conn, void* buffer, size_t len) {

        size_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {

            auto sent_bytes = send(conn->getSocket(), ptr + offset, len - offset, 0);

            // Stop if send didn't work
            if (sent_bytes == -1) {
                net_errno = errno;
                return false;
            }

            offset += sent_bytes;

        } while (offset < len);

        return true;
    }

    ssize_t receiveRaw(const Connection *conn, void *buffer, size_t min_len) {

        ssize_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {

            auto recv_bytes = recv(conn->getSocket(), ptr + offset, min_len - offset, MSG_DONTWAIT);

            if (recv_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            if (recv_bytes == 0) {
                net_errno = MIMP_DISCONNECT;
                return 0;
            };

            if (recv_bytes == -1) {
                net_errno = errno;
                return -1;
            }

            offset += recv_bytes;

        } while (offset < min_len);

        return offset;
    }

    bool sendEncrypted(const Connection *conn, void *buffer, const uint16_t len) {

        uint8_t header  [MIMP_HEADER_BYTES              ];
        uint8_t payload [crypto_secretbox_MACBYTES + len];

        // Magic Byte
        header[0] = MIMP_MAGIC_BYTE;

        // Version Byte
        header[1] = MIMP_VERSION;

        // Length Bytes (Big Endian)
        uint16_t final_data_len = sizeof(payload);
        if (final_data_len < len) {
            return false; // Message too large
        }
        header[2] = final_data_len >> 8    ; // 8 MSBs
        header[3] = final_data_len & 0xFF  ; // 8 LSBs

        // Nonce
        uint8_t nonce[crypto_secretbox_NONCEBYTES];
        randombytes_buf(nonce, sizeof(nonce));
        memcpy(header + 4, nonce, crypto_secretbox_NONCEBYTES);

        // Encrypt message into payload
        crypto_secretbox_easy(payload, static_cast<uint8_t *>(buffer), len, nonce, conn->sessionKey());

        // Assemble whole message (header || payload)
        uint8_t message_buffer[sizeof(header) + sizeof(payload)];
        memcpy(message_buffer, header, sizeof(header));
        memcpy(message_buffer + sizeof(header), payload, sizeof(payload));

        // Send message to client
        return sendRaw(conn, message_buffer, sizeof(message_buffer));
    }

    std::vector<uint8_t>* receiveEncrypted(const Connection *conn) {

        uint8_t temp[DEFAULT_BUFFER_SIZE];
        std::vector<uint8_t> msg_buffer;
        size_t received_bytes = 0;

        // Zero temp buffer
        sodium_memzero(temp, sizeof(temp));

        // Get (at least) the header
        do {

            // Receive bytes from socket
            ssize_t recv_bytes = receiveRaw(conn, temp, 0);
            if (recv_bytes <= 0) return nullptr;

            // Add those bytes to the overall message buffer
            received_bytes += recv_bytes;
            msg_buffer.resize(msg_buffer.size() + received_bytes);
            memcpy(msg_buffer.data() + received_bytes, temp, recv_bytes);

        } while (received_bytes < MIMP_HEADER_BYTES);

        // Calculate the size of the message data
        uint16_t data_len = msg_buffer[2] << 8 | msg_buffer[3];
        msg_buffer.resize(MIMP_HEADER_BYTES + data_len);

        // Continue to receive
        while (received_bytes < msg_buffer.size()) {

            // Receive bytes from socket
            ssize_t recv_bytes = receiveRaw(conn, temp, 0);
            if (recv_bytes <= 0) return nullptr;

            // Add those bytes to the overall message buffer
            received_bytes += recv_bytes;
            msg_buffer.resize(msg_buffer.size() + received_bytes);
            memcpy(msg_buffer.data() + received_bytes, temp, recv_bytes);

        }

        // Place nonce into a separate buffer
        uint8_t nonce[crypto_secretbox_NONCEBYTES];
        memcpy(nonce, msg_buffer.data() + 4, crypto_secretbox_NONCEBYTES);

        // Place ciphertext into a separate buffer
        uint8_t ciphertext[data_len];
        memcpy(ciphertext, msg_buffer.data() + MIMP_HEADER_BYTES, data_len);

        // Decrypt message
        uint8_t plaintext[data_len - crypto_secretbox_MACBYTES];
        if (crypto_secretbox_open_easy(plaintext, ciphertext, data_len, nonce, conn->sessionKey()) != 0) {
            std::cout << "Forged message detected! Dropping..." << std::endl;
            return nullptr;
        }

        // Return plaintext as vector
        auto* return_buffer = new std::vector<uint8_t>(sizeof(plaintext));
        memcpy(return_buffer->data(), plaintext, sizeof(plaintext));
        return return_buffer;

    }
}
