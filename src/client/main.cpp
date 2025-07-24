#include <csignal>
#include <iostream>
#include <strings.h>
#include <thread>
#include <netinet/in.h>

#include "common/constants.h"
#include "client/Client.h"
#include "common/util/config.h"

std::atomic<bool> is_running;

void startClient(util::DBConfig config) {

    is_running = true;

    std::this_thread::sleep_for(std::chrono::milliseconds(500 ));

    std::cout << std::endl << "Starting client and Handshaking server..." << std::endl;

    // Create address
    sockaddr_in addr{};
    bzero((char*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEFAULT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    MiniDB::Client client{addr, config.secret, is_running};
    client.connectToServer();

    while (is_running) {

        std::string msg;
        std::cout << "Enter message: " << std::endl;
        std::getline(std::cin, msg);

        if (msg.size() == 0) {
            is_running = false;
        } else {
            client.send_message(msg);
        }

    }

}

void handle_close_signal(int sig) {
    is_running = false;
}

int main(int argc, char* argv[]) {

    // Close Signal Handler
    signal(SIGINT, handle_close_signal);

    // Print intro
    std::cout << "================== MINI DB CLIENT ==================" << std::endl;
    std::cout << "Press Ctrl+C to quit." << std::endl;

    // Default arg values
    std::string config_file_path = DEFAULT_CONFIG_PATH;

    // Load CLI arguments
    int idx = 1;
    while (idx < argc) {
        if (argv[idx] == "--config") {
            config_file_path = argv[++idx];
            break;
        }
        idx++;
    }

    // Get database config
    std::cout << "Loading config from: " << config_file_path << std::endl;
    util::DBConfig config = util::get_config(config_file_path);

    startClient(config);

}