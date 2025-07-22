//
// Created by root on 7/22/25.
//

#ifndef NETWORKING_H
#define NETWORKING_H
#include <sys/types.h>

namespace util {

    bool    sendBytes(int sock, void* buffer, size_t len);
    ssize_t receiveBytes(int sock, void *buffer, size_t len);


}

#endif //NETWORKING_H
