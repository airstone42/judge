#ifndef CONTAINER_H
#define CONTAINER_H

#include "judgement.h"
#include "judge.h"

#include <string>
#include <deque>

namespace judgement {
    class Container {
    public:
        static Container &instance();

        Container(const Container &) = delete;

        Container &operator=(const Container &) = delete;

        Container(Container &&) = delete;

        Container &operator=(Container &&) = delete;

        const status_t &status();

        void handle(zmq::context_t &context);

        void reply(const std::string &message, zmq::socket_t &socket);

        void push(const zmq::message_t &request);

        void shift();

    private:
        Container() = default;

        std::deque<Judge> judges;
    };
}


#endif
