#include "container.h"

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>

#include <unistd.h>

namespace judgement {
    Container &Container::instance() {
        static Container container;
        return container;
    }

    const status_t &Container::status() {
        return this->judges.front().get_status();
    }

    void Container::handle(zmq::context_t &context) {
        zmq::socket_t socket(context, ZMQ_REP);
        socket.connect(INPROC_ADDRESS);
        while (true) {
            zmq::message_t request;
            socket.recv(&request);
            std::cout << "Received <" << request << ">" << std::endl;
            if (!request.size())
                return;
            std::lock_guard<std::mutex> lock(this->mutex);
            this->push(request);
            this->threads.back().join();
            this->reply(status_message(this->status()), socket);
            this->shift();
        }
    }

    void Container::reply(const std::string &message, zmq::socket_t &socket) {
        std::cout << message << std::endl;
        zmq::message_t reply(message.size());
        memcpy(reply.data(), message.data(), message.size());
        socket.send(reply);
    }

    void Container::push(const zmq::message_t &request) {
        std::string str = std::string(static_cast<const char *>(request.data()), request.size());
        if (str.empty() || str == " ")
            return;
        this->judges.emplace_back(Judge(split(str)));
        this->threads.emplace_back(std::thread(&Judge::run, &this->judges.back()));
    }

    void Container::shift() {
        this->judges.pop_front();
        this->threads.pop_front();
    }
}