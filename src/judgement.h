#ifndef JUDGEMENT_H
#define JUDGEMENT_H

#include <zmq.hpp>

#include <cstdint>
#include <string>
#include <chrono>

namespace judgement {
    extern const char *const TCP_ADDRESS;
    extern const char *const INPROC_ADDRESS;
    extern const int MAX_WORKERS;
    extern const int TIME_LIMIT;
    extern const char *const C_COMPILER;
    extern const char *const CXX_COMPILER;

    enum class ext_t : std::uint8_t {
        C,
        CXX,
        Other
    };

    enum class status_t : std::uint8_t {
        Waiting,
        InputError,
        SourceTypeError,
        FileNotFound,
        ExceptionOccurred,
        CompileError,
        RuntimeError,
        LimitExceed,
        WrongAnswer,
        Accepted
    };

    struct source_t {
        std::string name;
        std::string ext;
        ext_t ext_type;
    };

    struct input_t : source_t {
        long id = 0;
    };

    input_t split(const std::string &input);

    std::string status_message(const status_t &status);

    std::string time_message(const std::chrono::milliseconds &duration);
}

#endif
