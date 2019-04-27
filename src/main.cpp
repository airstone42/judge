#include "judgement.h"
#include "container.h"

#include <thread>
#include <iostream>
#include <functional>

int main(int argc, char *argv[]) {
    judgement::Container &container = judgement::Container::instance();
    zmq::context_t context(1);
    zmq::socket_t master(context, ZMQ_ROUTER);
    master.bind(judgement::TCP_ADDRESS);
    zmq::socket_t worker(context, ZMQ_DEALER);
    worker.bind(judgement::INPROC_ADDRESS);
    std::cout << "Listening on "
              << judgement::TCP_ADDRESS
              << "..."
              << std::endl;
    for (int i = 0; i < judgement::MAX_WORKERS; i++)
        std::thread(&judgement::Container::handle, std::ref(container), std::ref(context)).detach();
    zmq::proxy(static_cast<void *>(master), static_cast<void *>(worker), nullptr);
    return 0;
}
