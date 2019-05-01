#ifndef CONTAINER_H
#define CONTAINER_H

#include "judge.h"

#include <mutex>
#include <map>
#include <atomic>

namespace judge {
    class Container final {
    public:
        static Container &instance();

        Container(const Container &) = delete;

        Container &operator=(const Container &) = delete;

        Container(Container &&) = delete;

        Container &operator=(Container &&) = delete;

        const status_t &status(double id) const;

        const std::chrono::milliseconds &compiling_time(double id) const;

        const std::chrono::milliseconds &executing_time(double id) const;

        long executing_memory(double id) const;

        void handle(zmq::context_t &context);

        void run(const zmq::message_t &request, zmq::socket_t &socket);

        void insert(double &offset, const input_t &input);

        void reply(const std::string &message, zmq::socket_t &socket);

    private:
        Container() = default;

        std::mutex mutex;
        std::map<double, Judge> judges;
        std::atomic_int working = 0;
    };
}

#endif
