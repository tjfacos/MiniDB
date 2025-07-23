//
// Created by root on 7/22/25.
//

#ifndef NETWORKING_H
#define NETWORKING_H

#include <vector>
#include <sys/types.h>

#include "Connection.h"

namespace util {

    bool    sendRaw     (const Connection *conn, void* buffer, size_t len);
    ssize_t receiveRaw  (const Connection *conn, void *buffer, size_t min_len);

    bool                    sendEncrypted   (const Connection *conn, void *buffer, uint16_t len, uint8_t session_key[]);
    std::vector<uint8_t>*   receiveEncrypted(const Connection *conn, uint8_t session_key[]);

}

#endif //NETWORKING_H
