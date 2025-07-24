
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <netinet/in.h>

#include "common/Connection.h"

/* NOTE: If we use this class as part of the public API,
 * I need to make the socket cross--platform (reed, bugger
 * about with Windows).
 *
 */

namespace MiniDB {

class Client {

    std::string secret;
    Connection* conn;

    public:
        Client(sockaddr_in server_addr, std::string& secret);
        ~Client();
        void connectToServer();
        bool authenticate();

    void AwaitServerReady() const;
    void AwaitServerAcknowledgement() const;

    void            send_message(std::string msg) const;
        std::string recv_message() const;
};

} // MiniDB

#endif //CLIENT_H
