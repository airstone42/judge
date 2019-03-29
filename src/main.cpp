#include "judgement.h"
#include "container.h"

#include <thread>
#include <iostream>
#include <functional>

using namespace std;
using namespace judgement;
using namespace zmq;

int main(int argc, char *argv[]) {
    Container &container = Container::instance();
    context_t context(1);
    socket_t master(context, ZMQ_ROUTER);
    master.bind(TCP_ADDRESS);
    socket_t worker(context, ZMQ_DEALER);
    worker.bind(INPROC_ADDRESS);
    std::cout << "Listening..." << std::endl;
    for (int i = 0; i < MAX_WORKERS; i++)
        thread(&Container::handle, ref(container), ref(context)).detach();
    zmq::proxy(master, worker, nullptr);
    return 0;
}
