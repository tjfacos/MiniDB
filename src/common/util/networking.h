//
// Created by root on 7/22/25.
//

#ifndef NETWORKING_H
#define NETWORKING_H

#include <atomic>
#include <vector>

#include "common/Connection.h"

#define MIMP_DISCONNECT     2001

namespace util {
    inline int net_errno;

    /**
     * @brief Send raw bytes to a recipient
     * @param conn Connection to recipient
     * @param buffer Data to send
     * @param len Number of bytes to send
     * @param carry_on Flag to continue sending data
     * @return true if the operation was successful, false otherwise
     */
    bool sendRaw(const Connection *conn, void* buffer, size_t len, std::atomic<bool> &carry_on);

    /**
     * @brief Receive data from a connection.
     * @note This will only read out n bytes. If there are more than n bytes on the socket,
     * they remain there. If there's less than n bytes on the socket, strange things occur...
     * (the function remains in a loop until it receives n bytes)
     * @param conn Connection to sender
     * @param buffer Buffer to place received data
     * @param bytes_to_return Number of bytes to return
     * @param carry_on Flag to continue receiving data
     * @return Number of bytes read into buffer, or -1 in case of an error.
     * If an error occurs, util::net_errno is set to describe the error encountered.
     */
    ssize_t receiveRaw(const Connection *conn, void *buffer, size_t bytes_to_return, std::atomic<bool> &carry_on);


    /**
     * @brief Send data using encrypted MIMP protocol (see NOTES.md)
     * @param conn Connection to send the data to
     * @param buffer The data buffer
     * @param len The number of bytes to send
     * @param carry_on Flag to continue sending data
     * @return True if the data was successfully sent, false otherwise.
     */
    bool sendEncrypted(const Connection *conn, void *buffer, uint16_t len, std::atomic<bool> &carry_on);

    /**
     * @brief Receives an encrypted message (secured with MIMP), decrypts it, and returns the plaintext message
     * @param conn Connection to receive message from
     * @param carry_on Flag to continue receiving data
     * @return A vector containing the message
     */
    std::vector<uint8_t>* receiveEncrypted(const Connection *conn, std::atomic<bool> &carry_on);

    /**
     * @brief Checks if a connection is still alive (the socket at the other end is still open)
     * @param conn Connection
     * @return True if the other socket is still open, false otherwise
     */
    bool connectionIsAlive(const Connection *conn);

}

#endif //NETWORKING_H
