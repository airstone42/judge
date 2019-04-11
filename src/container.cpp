#include "container.h"

#include <iostream>
#include <thread>
#include <string>
#include <random>
#include <mutex>
#include <utility>
#include <cstring>
#include <stdexcept>

#include <unistd.h>

namespace judgement {
    Container &Container::instance() {
        static Container container;
        return container;
    }

    const status_t &Container::status(double id) const {
        return this->judges.at(id).get_status();
    }

    const std::chrono::milliseconds &Container::compiling_time(double id) const {
        return this->judges.at(id).get_compiling_time();
    }

    const std::chrono::milliseconds &Container::executing_time(double id) const {
        return this->judges.at(id).get_executing_time();
    }

    long Container::executing_memory(double id) const {
        return this->judges.at(id).get_executing_memory();
    }

    void Container::handle(zmq::context_t &context) {
        zmq::socket_t socket(context, ZMQ_REP);
        socket.connect(INPROC_ADDRESS);
        while (true) {
            zmq::message_t request;
            socket.recv(&request);
            std::cout << "=====>> Received <" << std::string(static_cast<char *>(request.data()), request.size()) << ">"
                      << std::endl;
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
        double offset = input.id;
        this->insert(offset, input);
        std::string message = std::to_string(input.id) + ":";
        try {
            this->judges.at(offset).run(offset);
            message += status_message(this->status(offset)) + ":";
            message += time_message(this->compiling_time(offset)) + "ms:";
            message += time_message(this->executing_time(offset)) + "ms:";
            message += std::to_string(this->executing_memory(offset)) + "kB";
        } catch (std::out_of_range &) {
            message += status_message(status_t::E) + ":0ms:0ms:0kB";
        }
        this->reply(message, socket);
        std::lock_guard<std::mutex> lock(this->mutex);
        this->judges.erase(offset);
    }

    void Container::insert(double &offset, const input_t &input) {
        std::lock_guard<std::mutex> lock(this->mutex);
        do {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<> dist(0, 1);
            offset += dist(mt);
        } while (this->judges.find(input.id) != this->judges.end());
        this->judges.insert(std::make_pair(offset, Judge({input.io_path, input.code_path, input.ext, input.ext_type})));
    }

    void Container::reply(const std::string &message, zmq::socket_t &socket) {
        std::cout << "<<===== " + message << std::endl;
        zmq::message_t reply(message.size());
        memcpy(reply.data(), message.data(), message.size());
        socket.send(reply);
    }
}
