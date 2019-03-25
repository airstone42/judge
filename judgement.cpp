#include "judgement.h"

#include <string>

namespace judgement {
    const char *const TCP_ADDRESS = "tcp://*:5555";
    const char *const INPROC_ADDRESS = "inproc://worker";
    const int MAX_WORKERS = 10;
    const char *const C_COMPILER = "gcc";
    const char *const CXX_COMPILER = "g++";

    input_t split(const std::string &input) {
        std::string delimiter = " ";
        if (input.find(delimiter) == std::string::npos) {
            return input_t{input, "c", source_t::CSource};
        } else {
            std::string filename = input.substr(0, input.find(delimiter));
            std::string ext = input.substr(input.find(delimiter) + 1, input.size() - input.find(delimiter));
            source_t source_type;
            if (ext == "c")
                source_type = source_t::CSource;
            else if (ext == "cpp" || ext == "cxx" || ext == "cc")
                source_type = source_t::CXXSource;
            else
                source_type = source_t::Other;
            return input_t{filename, ext, source_type};
        }
    }

    std::string status_message(const status_t &status) {
        switch (status) {
            case status_t::Waiting:
                return "Waiting";
            case status_t::InputError:
                return "Input error!";
            case status_t::SourceTypeError:
                return "Source type error!";
            case status_t::FileNotFound:
                return "File not found!";
            case status_t::CompileError:
                return "Compiling error!";
            case status_t::Executed:
                return "Executed!";
            case status_t::RuntimeError:
                return "Runtime error!";
            case status_t::LimitExceed:
                return "Limit exceed!";
            case status_t::WrongAnswer:
                return "Wrong answer!";
            case status_t::Accepted:
                return "Accepted!";
            default:
                return std::string();
        }
    }
}
