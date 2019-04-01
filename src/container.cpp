#include "container.h"

#include <iostream>
#include <thread>
#include <string>
#include <random>
#include <utility>
#include <cstring>

#include <unistd.h>

namespace judgement {
    Container &Container::instance() {
        static Container container;
        return container;
    }

    const status_t &Container::status(int id) const {
        return this->judges.at(id).get_status();
    }

    const std::chrono::milliseconds &Container::compiling_time(int id) const {
        return this->judges.at(id).get_compiling_time();
    }

    const std::chrono::milliseconds &Container::executing_time(int id) const {
        return this->judges.at(id).get_executing_time();
    }

    long Container::executing_memory(int id) const {
        return this->judges.at(id).get_executing_memory();
    }

    void Container::handle(zmq::context_t &context) {
        zmq::socket_t socket(context, ZMQ_REP);
        socket.connect(INPROC_ADDRESS);
        while (true) {
            zmq::message_t request;
            socket.recv(&request);
            std::cout << ">> Received <" << request << ">" << std::endl;
            if (!request.size())
                return;
            this->run(request, socket);
        }
    }

    void Container::run(const zmq::message_t &request, zmq::socket_t &socket) {
        std::string str = std::string(static_cast<const char *>(request.data()), request.size());
        if (str.empty() || str == " ")
            return;
        input_t input = split(str);
        int offset = input.id;
        while (this->judges.find(input.id) != this->judges.end()) {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<> dist(1, 100);
            offset += dist(mt);
        }
        this->judges.insert(std::make_pair(offset, Judge({input.name, input.ext, input.ext_type})));
        this->judges.at(offset).run();
        std::string message = std::to_string(input.id) + ":";
        message += status_message(this->status(offset)) + ":";
        message += time_message(this->compiling_time(offset)) + "ms:";
        message += time_message(this->executing_time(offset)) + "ms:";
        message += std::to_string(this->executing_memory(offset)) + "kB";
        std::cout << "<< " + message << std::endl;
        zmq::message_t reply(message.size());
        memcpy(reply.data(), message.data(), message.size());
        socket.send(reply);
        this->judges.erase(offset);
    }
}
