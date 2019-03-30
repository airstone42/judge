#ifndef CONTAINER_H
#define CONTAINER_H

#include "judgement.h"
#include "judge.h"

#include <chrono>
#include <map>

namespace judgement {
    class Container {
    public:
        static Container &instance();

        Container(const Container &) = delete;

        Container &operator=(const Container &) = delete;

        Container(Container &&) = delete;

        Container &operator=(Container &&) = delete;

        const status_t &status(int id) const;

        const std::chrono::milliseconds &compiling_time(int id) const;

        const std::chrono::milliseconds &running_time(int id) const;

        void handle(zmq::context_t &context);

        void run(const zmq::message_t &request, zmq::socket_t &socket);

    private:
        Container() = default;

        std::map<int, Judge> judges;
    };
}


#endif
