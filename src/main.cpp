#include <csignal>
#include <iostream>
#include <sodium.h>
#include <strings.h>
#include <thread>

#include "client/Client.h"
#include "server/Server.h"
#include "util/functions.h"

#define DEFAULT_PORT            12345
#define DEFAULT_CONFIG_PATH     "minidb.conf"
#define DEFAULT_THREADS         1

class Main {

    std::atomic<bool> is_running;

    Server server;
    std::thread server_thread;

    public:

        Main(): is_running(false), server(is_running) {}

        ~Main() {
            if (is_running) halt();
            if (server_thread.joinable()) server_thread.join();
        };

        void halt() {
            if (!is_running) return;
            std::cout << std::endl << "CLOSING..." << std::endl;
            is_running = false;
        }

        void start(util::DBConfig config) {

            is_running = true;

            // Initialise Sodium
            if (sodium_init() < 0)
                util::error("ERROR: Failed to initialize libsodium.");

            // Set server secret
            server.setSecret(config.secret);

            // Start Server
            std::cout << "Starting server..." << std::endl;
            server_thread = std::thread(&Server::run, &server, DEFAULT_PORT);
            server_thread.join();

        }
};

Main m{};

void handle_close_signal(int sig) {
    m.halt();
}

void testHandshake(std::string secret) {

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 ));

    std::cout << std::endl << "Starting client and Handshaking server..." << std::endl;

    // Create address
    sockaddr_in addr{};
    bzero((char*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEFAULT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // std::string wrong_secret = "TheWrongSecret!!!";

    MiniDB::Client client{addr, secret};
    client.test();

}

int main(int argc, char* argv[]) {

    // Close Signal Handler
    signal(SIGINT, handle_close_signal);

    std::cout << "================== MINI DB ==================";
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

    // Start Server and Client
    std::thread server_thread{&Main::start, &m, config};
    std::thread client_thread{testHandshake, config.secret};

    client_thread.join();
    server_thread.join();

}
