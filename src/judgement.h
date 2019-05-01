#ifndef JUDGEMENT_H
#define JUDGEMENT_H

#include <zmq.hpp>

#include <cstdint>
#include <string>
#include <chrono>

#include <sys/time.h>

namespace judgement {
    extern const char *const TCP_ADDRESS;
    extern const char *const INPROC_ADDRESS;
    extern const int MAX_THREADS;
    extern const int MAX_WORKS;
    extern const int TIME_LIMIT_MS;
    extern const int TIME_INTERVAL_MS;
    extern const char *const C_COMPILER;
    extern const char *const CXX_COMPILER;
    extern const char *const NULL_PATH;
    extern const char *const SECCOMP_SOURCE;
    extern const char *const DL;
    extern const char *const SECCOMP;

    enum class ext_t : std::uint8_t {
        C,
        CXX,
        Other
    };

    enum class status_t : std::uint8_t {
        W,
        IE,
        TE,
        NF,
        E,
        CE,
        RE,
        LE,
        WA,
        AC
    };

    struct source_t {
        std::string io_path;
        std::string code_path;
        std::string ext;
        ext_t ext_type;
    };

    struct input_t : source_t {
        long id = 0;
    };

    input_t split(const std::string &input, const std::string &delimiter = ":");

    std::string status_message(const status_t &status);

    std::string time_message(const std::chrono::milliseconds &duration);

    std::chrono::milliseconds time_cast(const timeval &timeval);
}

#endif
