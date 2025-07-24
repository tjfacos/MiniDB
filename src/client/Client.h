
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <netinet/in.h>

#include "common/Connection.h"
#include "common/Haltable.h"

/* NOTE: If we use this class as part of the public API,
 * I need to make the socket cross--platform (reed, bugger
 * about with Windows).
 *
 */

namespace MiniDB {

class Client : public util::Haltable {

    std::string secret;
    Connection* conn;

    public:
        Client(sockaddr_in server_addr, std::string& secret, std::atomic<bool>& is_running);
        ~Client();
        void connectToServer();
        bool authenticate();

    void AwaitServerReady() const;
    void AwaitServerAcknowledgement() const;

    void send_message(std::string msg) const;
    std::string recv_message() const;
};

}

#endif
