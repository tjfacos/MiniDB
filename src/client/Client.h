
#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <netinet/in.h>
#include <sodium.h>

/* NOTE: If we use this class as part of the public API,
 * I need to make the socket cross--platform (reed, bugger
 * about with Windows).
 *
 */

namespace MiniDB {

class Client {

    sockaddr_in server_addr ;
    std::string secret      ;
    uint8_t session_key[crypto_auth_hmacsha512_BYTES] ;
    int sock;

    public:
        Client(sockaddr_in server_addr, std::string& secret);
        ~Client();
        void test();
        bool authenticate();
        // ssize_t     send_message(void* buffer, size_t length);
        // uint8_t*    recv_message();
};

} // MiniDB

#endif //CLIENT_H
