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

#include "util/logging.h"

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
                report(conn, "Send failed! Oh bugger...");
                net_errno = errno;
                return false;
            }

            offset += sent_bytes;

        } while (offset < len);

        return true;
    }

    ssize_t receiveRaw(const Connection *conn, void *buffer, size_t bytes_to_return) {

        ssize_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {


            auto recv_bytes
                = recv(conn->getSocket(), ptr + offset, bytes_to_return - offset, MSG_DONTWAIT);

            if (recv_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            if (recv_bytes == 0) {
                net_errno = MIMP_DISCONNECT;
                return 0;
            };

            if (recv_bytes == -1) {
                report(conn, "receiveRaw: an unknown error has occurred.");
                net_errno = errno;
                return -1;
            }

            offset += recv_bytes;

        } while (offset < bytes_to_return);

        report(conn, "Read from socket for [" + conn->getEndpoint() + "] successful! Read " + std::to_string(offset) + " bytes.");

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

        report(conn, "================ SENDING MIMP MESSAGE ================");
        report(conn, "Plaintext message size: " + std::to_string(len));
        report(conn, "Payload Size: " + std::to_string(final_data_len) + " bytes (Encoded value: " + std::to_string(header[2] << 8 | header[3]) + ")");
        report(conn, "Total message size (Payload + Header): " + std::to_string(sizeof(message_buffer)));
        report(conn, "================ SENDING MIMP MESSAGE ================");

        // Send message to client
        return sendRaw(conn, message_buffer, sizeof(message_buffer));
    }

    std::vector<uint8_t>* receiveEncrypted(const Connection *conn) {

        // Temp buffer for header
        ssize_t recv_bytes;
        uint8_t header_buff[MIMP_HEADER_BYTES];
        sodium_memzero(header_buff, sizeof(header_buff));

        // Get the header
        recv_bytes = receiveRaw(conn, header_buff, MIMP_HEADER_BYTES);
        if (recv_bytes <= 0) return nullptr;

        // Calculate the size of the message data
        uint16_t data_len = header_buff[2] << 8 | header_buff[3];

        // Temp buffer for payload
        uint8_t payload_buff[data_len];
        sodium_memzero(payload_buff, sizeof(payload_buff));

        // Get the payload
        recv_bytes = receiveRaw(conn, payload_buff, sizeof(payload_buff));
        if (recv_bytes <= 0) return nullptr;

        // Place nonce into a separate buffer
        uint8_t nonce[crypto_secretbox_NONCEBYTES];
        memcpy(nonce, header_buff + 4, crypto_secretbox_NONCEBYTES);

        // Decrypt payload
        uint16_t plaintext_len = data_len - crypto_secretbox_MACBYTES;
        uint8_t plaintext[plaintext_len];
        if (crypto_secretbox_open_easy(plaintext, payload_buff, data_len, nonce, conn->sessionKey()) != 0) {
            std::cout << "Forged message detected! Dropping..." << std::endl;
            return nullptr;
        }

        // Return plaintext as vector
        auto* return_buffer = new std::vector<uint8_t>(sizeof(plaintext));
        memcpy(return_buffer->data(), plaintext, sizeof(plaintext));
        return return_buffer;

    }
}
