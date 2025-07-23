//
// Created by root on 7/22/25.
//

#ifndef NETWORKING_H
#define NETWORKING_H

#include <vector>
#include <sys/types.h>

#include "Connection.h"

#define MIMP_DISCONNECT     2001

namespace util {
    inline int net_errno;

    bool    sendRaw     (const Connection *conn, void* buffer, size_t len);
    ssize_t receiveRaw  (const Connection *conn, void *buffer, size_t max_to_return);

    bool                    sendEncrypted   (const Connection *conn, void *buffer, uint16_t len);
    std::vector<uint8_t>*   receiveEncrypted(const Connection *conn);

}

#endif //NETWORKING_H
