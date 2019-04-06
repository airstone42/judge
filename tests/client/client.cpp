#include "client.h"

#include <iostream>
#include <string>
#include <thread>
#include <cstring>

using namespace std;
using namespace zmq;

const char *const TCP_ADDRESS = "tcp://localhost:5555";

int main() {
    context_t context(1);
    socket_t socket(context, ZMQ_REQ);
    socket.connect(TCP_ADDRESS);
    cout << "Connecting to server…" << endl;
    string input;
    while (getline(cin, input) && !input.empty() && input != " ") {
        run(input, socket);
    }
    return 0;
}

void run(const string &input, socket_t &socket) {
    message_t request(input.size());
    memcpy(request.data(), input.data(), input.size());
    cout << "Sending: <" << string(static_cast<char *>(request.data()), request.size()) << ">" << endl;
    socket.send(request);
    message_t reply;
    socket.recv(&reply);
    cout << "Result: <" << string(static_cast<char *>(reply.data()), reply.size()) << ">" << endl;
}

