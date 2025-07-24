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

    char msg[] =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam ut mauris purus. Praesent vitae arcu interdum, dapibus purus vitae, ultrices elit. Sed efficitur interdum sem. Donec ut magna augue. Fusce tincidunt laoreet tristique. Vivamus lacinia nisi in enim tincidunt, nec convallis tortor volutpat. Praesent ultricies quis est eget pulvinar. Nulla gravida nec velit a fringilla. Cras sed volutpat neque. Quisque euismod enim nisl, in luctus sem mattis sollicitudin. Ut in purus ipsum."
        "Praesent laoreet diam sit amet ipsum ullamcorper faucibus. Pellentesque at lectus interdum, semper orci a, scelerisque felis. Maecenas feugiat ante nec viverra ultrices. Aenean et nisi ac urna laoreet aliquet. Maecenas id purus sed tellus facilisis porta. Sed tincidunt orci est, eget fringilla ex tincidunt pulvinar. Morbi imperdiet quam ex, eget pellentesque metus commodo sit amet. Proin malesuada pharetra vestibulum. Nulla imperdiet cursus mauris, eu ullamcorper odio molestie eu. Phasellus fringilla lectus eget ultrices fermentum."
        "Integer purus metus, auctor ac feugiat ac, efficitur et mi. Cras aliquet turpis tortor, vel dictum mi venenatis pulvinar. Etiam ante neque, facilisis porttitor felis vitae, convallis tristique justo. Nullam sollicitudin vehicula blandit. Sed ex nibh, mollis et sapien ullamcorper, vestibulum accumsan magna. Sed fermentum nulla non rutrum dapibus. Nulla sagittis sem sed augue commodo, at porttitor sem ultrices. Nam cursus sed felis non finibus. Suspendisse aliquet placerat nisi, nec pharetra nisi lobortis in. Donec pulvinar fringilla ante vel hendrerit. Donec rutrum eget ipsum vitae efficitur."
        "Suspendisse eget purus libero. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nam nec magna pellentesque, accumsan dolor ornare, sagittis massa. Ut pretium justo turpis, eget vehicula felis suscipit quis. Fusce aliquet eget tortor vel vehicula. Vestibulum pulvinar libero eu ipsum mattis, ac ullamcorper erat consequat. Donec tincidunt ultricies lorem, ac eleifend ligula vehicula vitae. Curabitur viverra nibh sed velit dignissim sodales. In id tellus ut enim aliquam luctus. Sed tempor risus et nisl eleifend, eu pellentesque dui hendrerit. Phasellus vehicula eu ligula ut auctor. Interdum et malesuada fames ac ante ipsum primis in faucibus. Integer non libero ac mi congue fermentum ac sed mi. Donec a dolor sapien. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus."
        "Vivamus et gravida turpis. Aliquam pellentesque suscipit venenatis. Donec tincidunt, lorem at tincidunt imperdiet, nunc justo pulvinar nisl, sit amet finibus massa tortor at mauris. Nam mi erat, lacinia vitae neque sit amet, congue semper augue. Duis vulputate, velit ut rutrum ornare, turpis est tempus tellus, ac sodales nisi mi vel massa. Morbi cursus erat sed ultricies maximus. Vestibulum luctus leo nibh, sit amet placerat dui posuere sed. Donec vestibulum vestibulum tellus eu sagittis."
        "In at sem et arcu pulvinar aliquam in vel tortor. Vestibulum nisl nisl, venenatis nec commodo aliquet, ultrices a erat. Aenean ornare porta dolor, at pellentesque nulla pulvinar ut. In hac habitasse platea dictumst. Phasellus vitae tellus ac massa consectetur commodo. Suspendisse ipsum neque, laoreet a justo non, iaculis pulvinar sem. In volutpat nulla ut dui dignissim ultrices. Morbi ac nulla at nulla laoreet auctor. Nunc libero.";

    client.send_message(msg);
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