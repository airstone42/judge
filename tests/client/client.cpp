#include "client.h"

#include <iostream>
#include <string>
#include <csignal>
#include <cstring>

#include <unistd.h>

using namespace std;
using namespace zmq;

int main() {
    context_t context(1);
    socket_t socket(context, ZMQ_REQ);
    socket.connect(TCP_ADDRESS);
    cout << "Connecting to server…" << endl;
    string input;
    while (getline(cin, input) && !input.empty() && input != " ") {
        signal(SIGALRM, alarm_handler);
        alarm(limit);
        int result = run(input, socket);
        if (timeout && result)
            exit(0);
        else if (timeout)
            timeout = 0;
    }
    return 0;
}

void alarm_handler(int sig) {
    if (sig == SIGALRM)
        timeout = 1;
}

int run(const string &input, socket_t &socket) {
    message_t request(input.size());
    memcpy(request.data(), input.data(), input.size());
    cout << "Sending: <" << string(static_cast<char *>(request.data()), request.size()) << ">" << endl;
    socket.send(request);
    message_t reply;
    socket.recv(&reply);
    cout << "Result: <" << string(static_cast<char *>(reply.data()), reply.size()) << ">" << endl;
    return 0;
}
