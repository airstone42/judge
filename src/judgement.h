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
    extern const char *const C_COMPILER;
    extern const char *const CXX_COMPILER;

    enum class source_t : std::uint8_t {
        CSource,
        CXXSource,
        Other
    };

    enum class status_t : std::uint8_t {
        Waiting,
        InputError,
        SourceTypeError,
        FileNotFound,
        CompileError,
        Executed,
        RuntimeError,
        LimitExceed,
        WrongAnswer,
        Accepted
    };

    struct judge_t {
        std::string name;
        std::string ext;
        source_t source_type;
    };

    struct input_t : judge_t {
        int id = 0;
    };

    input_t split(const std::string &input);

    std::string status_message(const status_t &status);

    std::string time_message(const std::chrono::milliseconds &duration);
}

#endif
