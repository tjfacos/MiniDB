#include <csignal>
#include <iostream>

#include "server/Server.h"

#define DEFAULT_PORT 12345

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

        void start() {
            server_thread = std::thread(&Server::run, &server, DEFAULT_PORT);
            server_thread.join();
        }
};

Main m{};

void handle_close_signal(int sig) {
    m.halt();
}

int main() {
    signal(SIGINT, handle_close_signal);
    std::cout << "Starting server..." << std::endl;
    std::cout << "Press Ctrl+C to quit." << std::endl;
    m.start();
}
