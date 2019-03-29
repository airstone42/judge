#include <zmq.hpp>

#include <iostream>
#include <string>
#include <cstring>

using namespace std;
using namespace zmq;

int main() {
    context_t context(1);
    socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");
    cout << "Connecting to server…" << endl;
    string str;
    while (getline(cin, str) && !str.empty() && str != " ") {
        message_t request(str.size());
        memcpy(request.data(), str.data(), str.size());
        cout << "Sending: <" << request << ">" << endl;
        socket.send(request);
        message_t reply;
        socket.recv(&reply);
        cout << "Result: <" << reply << ">" << endl;
    }
    return 0;
}
