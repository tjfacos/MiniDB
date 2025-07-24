#include <csignal>
#include <iostream>
#include <sodium.h>
#include <strings.h>
#include <thread>

#include "Server.h"

#include "common/constants.h"
#include "common/util/logging.h"
#include "common/util/config.h"
#include "common/util/networking.h"

#define DEFAULT_THREADS         1

std::atomic<bool> is_running;

 void start(Server server, util::DBConfig config) {

     is_running = true;

     // Initialise Sodium
     if (sodium_init() < 0)
         util::error("ERROR: Failed to initialize libsodium.");

     // Set server secret
     server.setSecret(config.secret);

     // Start Server
     std::cout << "Starting server..." << std::endl;
     std::thread server_thread(&Server::run, &server, DEFAULT_PORT);
     server_thread.join();
    std::cout << std::endl << "Server stopped..." << std::endl;

 }

void handle_close_signal(int sig) {
    is_running = false;
}

int main(int argc, char* argv[]) {

    // Close Signal Handler
    signal(SIGINT, handle_close_signal);

    // Print intro
    std::cout << "================== MINI DB ==================" << std::endl;
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

    start(Server{is_running}, config);

}
