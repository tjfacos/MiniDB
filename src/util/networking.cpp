//
// Created by root on 7/22/25.
//

#include "networking.h"

#include <cstdint>
#include <sys/socket.h>
#include <sys/types.h>

namespace util {

    bool sendBytes(int sock, void* buffer, size_t len) {

        size_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {

            auto sent_bytes = send(sock, ptr + offset, len - offset, 0);

            if (sent_bytes == -1) {
                return false;
            }

            offset += sent_bytes;

        } while (offset < len);

        return true;
    }

    ssize_t receiveBytes(int sock, void *buffer, size_t len) {

        ssize_t offset = 0;
        auto* ptr = static_cast<uint8_t *>(buffer);

        do {
            auto recv_bytes = recv(sock, ptr + offset, len - offset, 0);
            if (recv_bytes <= 0) return -1;
            offset += recv_bytes;
        } while (offset < len);

        return offset;
    }

}
