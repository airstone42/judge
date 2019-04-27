#include "judgement.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <chrono>

namespace judgement {
    const char *const TCP_ADDRESS = "tcp://*:5555";
    const char *const INPROC_ADDRESS = "inproc://worker";
    const int MAX_WORKERS = 50;
    const int TIME_LIMIT_MS = 1000;
    const int TIME_INTERVAL_MS = 20;
    const char *const C_COMPILER = "gcc";
    const char *const CXX_COMPILER = "g++";
    const char *const NULL_PATH = "/dev/null";
    const char *const SECCOMP_SOURCE = "src/seccomp/rules.c";
    const char *const DL = "-ldl";
    const char *const SECCOMP = "-lseccomp";

    input_t split(const std::string &input, const std::string &delimiter) {
        std::vector<std::string> vector;
        std::string::size_type pos = 0, now = 0;
        while ((pos = input.find(delimiter, now)) != std::string::npos) {
            vector.push_back(input.substr(now, pos - now));
            now = pos + delimiter.length();
        }
        vector.push_back(input.substr(now, pos - now));
        ext_t ext_type = ext_t::Other;
        switch (vector.size()) {
            case 4:
                if (vector[3] == "c")
                    ext_type = ext_t::C;
                else if (vector[3] == "cpp" || vector[3] == "cxx" || vector[3] == "cc")
                    ext_type = ext_t::CXX;
                else
                    ext_type = ext_t::Other;
                try {
                    return {vector[1], vector[2], vector[3], ext_type, std::stol(vector[0])};
                } catch (std::invalid_argument &) {
                    return {vector[1], vector[2], vector[3], ext_type, 0};
                }
            case 3:
                if (vector[2] == "c")
                    ext_type = ext_t::C;
                else if (vector[2] == "cpp" || vector[2] == "cxx" || vector[2] == "cc")
                    ext_type = ext_t::CXX;
                else
                    ext_type = ext_t::Other;
                try {
                    return {vector[1], vector[1], vector[2], ext_type, std::stol(vector[0])};
                } catch (std::invalid_argument &) {
                    return {vector[0], vector[1], vector[2], ext_type, 0};
                }
            default:
                return input_t{NULL_PATH, NULL_PATH, NULL_PATH, ext_type, 0};
        }
    }

    std::string status_message(const status_t &status) {
        switch (status) {
            case status_t::W:
                return "Waiting...";
            case status_t::IE:
                return "Input error!";
            case status_t::TE:
                return "Source type error!";
            case status_t::NF:
                return "File not found!";
            case status_t::E:
                return "Exception occurred!";
            case status_t::CE:
                return "Compilation error!";
            case status_t::RE:
                return "Runtime error!";
            case status_t::LE:
                return "Limit exceeded!";
            case status_t::WA:
                return "Wrong answer!";
            case status_t::AC:
                return "Accepted!";
            default:
                return std::string();
        }
    }

    std::string time_message(const std::chrono::milliseconds &duration) {
        return std::to_string(duration.count());
    }

    std::chrono::milliseconds time_cast(const timeval &timeval) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::seconds(timeval.tv_sec) + std::chrono::microseconds(timeval.tv_usec));
    }
}
