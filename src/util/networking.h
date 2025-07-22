//
// Created by root on 7/22/25.
//

#ifndef NETWORKING_H
#define NETWORKING_H
#include <cstdint>
#include <sys/types.h>

namespace util {

    bool    sendRaw     (int sock, void* buffer, size_t len);
    ssize_t receiveRaw  (int sock, void *buffer, size_t min_len);

    bool    sendEncrypted   (int sock, void *buffer, uint16_t len, uint8_t session_key[]);
    ssize_t receiveEncrypted(int sock, void *buffer, uint8_t session_key[]);

}

#endif //NETWORKING_H
