#include <csignal>
#include <iostream>
#include <sodium.h>

#include "server/Server.h"
#include "util/functions.h"

#define DEFAULT_PORT    12345
#define DEFAULT_CONFIG  "minidb.conf"
#define DEFAULT_THREADS 1

class Main {

    std::atomic<bool> is_running;

    Server server;
    std::thread server_thread;

    public:

        Main(): is_running(true), server(is_running) {}

        ~Main() {
            if (is_running) halt();
            if (server_thread.joinable()) server_thread.join();
        };

        void halt() {
            std::cout << std::endl << "CLOSING..." << std::endl;
            is_running = false;
        }

        void start(util::DBConfig config) {

            // Initialise Sodium
            if (sodium_init() < 0)
                util::error("ERROR: Failed to initialize libsodium.");

            // TODO: Remove this
            std::cout << "DB_SECRET: " << config.secret << std::endl;

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

int main(int argc, char* argv[]) {

    // Close Signal Handler
    signal(SIGINT, handle_close_signal);

    std::cout << "================== MINI DB ==================";
    std::cout << "Press Ctrl+C to quit." << std::endl;

    // Default arg values
    std::string config_file_path = DEFAULT_CONFIG;

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

    // Start DB Processes
    m.start(config);
}
