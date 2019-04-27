#include "client.h"

#include <iostream>
#include <string>
#include <csignal>
#include <cstring>

#include <unistd.h>

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(TCP_ADDRESS);
    std::cout << "Connecting to server "
              << TCP_ADDRESS
              << "..."
              << std::endl;
    std::string input;
    while (getline(std::cin, input) && !input.empty() && input != " ") {
        signal(SIGALRM, handler);
        alarm(limit);
        int result = run(input, socket);
        if (timeout && result)
            exit(0);
        else if (timeout)
            timeout = 0;
    }
    return 0;
}

void handler(int sig) {
    if (sig == SIGALRM)
        timeout = 1;
}

int run(const std::string &input, zmq::socket_t &socket) {
    zmq::message_t request(input.size());
    memcpy(request.data(), input.data(), input.size());
    std::cout << "Sending: <"
              << std::string(static_cast<char *>(request.data()), request.size())
              << ">"
              << std::endl;
    socket.send(request);
    zmq::message_t reply;
    socket.recv(&reply);
    std::cout << "Result: <"
              << std::string(static_cast<char *>(reply.data()), reply.size())
              << ">"
              << std::endl;
    return 0;
}
