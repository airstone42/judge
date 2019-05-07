#include "util.h"
#include "container.h"

#include <thread>
#include <iostream>
#include <functional>

int main(int argc, char *argv[]) {
    judge::Container &container = judge::Container::instance();
    zmq::context_t context(1);
    zmq::socket_t master(context, ZMQ_ROUTER);
    master.bind(judge::TCP_ADDRESS);
    zmq::socket_t worker(context, ZMQ_DEALER);
    worker.bind(judge::INPROC_ADDRESS);
    std::cout << "Listening on "
              << judge::TCP_ADDRESS
              << "..."
              << std::endl;
    for (int i = 0; i < judge::MAX_THREADS; ++i)
        std::thread(&judge::Container::handle, std::ref(container), std::ref(context)).detach();
    zmq::proxy(static_cast<void *>(master), static_cast<void *>(worker), nullptr);
    return 0;
}
