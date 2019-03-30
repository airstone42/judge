#include "judgement.h"

#include <string>
#include <stdexcept>

namespace judgement {
    const char *const TCP_ADDRESS = "tcp://*:5555";
    const char *const INPROC_ADDRESS = "inproc://worker";
    const int MAX_WORKERS = 50;
    const char *const C_COMPILER = "gcc";
    const char *const CXX_COMPILER = "g++";

    input_t split(const std::string &input) {
        std::string delimiter = ":";
        if (input.find(delimiter) == std::string::npos) {
            return input_t{input, "c", ext_t::C, 0};
        } else if (input.find(delimiter) == input.rfind(delimiter)) {
            try {
                std::string id = input.substr(0, input.find(delimiter));
                std::string name = input.substr(input.find(delimiter) + 1, input.size() - input.find(delimiter));
                return input_t{name, "c", ext_t::C, std::stoi(id)};
            } catch (std::out_of_range &) {
                std::string name = input.substr(input.find(delimiter) + 1, input.size() - input.find(delimiter));
                return input_t{name, "c", ext_t::C, 0};
            } catch (std::invalid_argument &) {
                std::string name = input.substr(0, input.find(delimiter));
                std::string ext = input.substr(input.find(delimiter) + 1, input.size() - input.find(delimiter));
                ext_t ext_type;
                if (ext == "c")
                    ext_type = ext_t::C;
                else if (ext == "cpp" || ext == "cxx" || ext == "cc")
                    ext_type = ext_t::CXX;
                else
                    ext_type = ext_t::Other;
                return input_t{name, ext, ext_type, 0};
            }
        } else {
            std::string id = input.substr(0, input.find(delimiter));
            std::string name = input.substr(input.find(delimiter) + 1, input.rfind(delimiter) - input.find(delimiter) - 1);
            std::string ext = input.substr(input.rfind(delimiter) + 1, input.size() - input.rfind(delimiter));
            ext_t ext_type;
            if (ext == "c")
                ext_type = ext_t::C;
            else if (ext == "cpp" || ext == "cxx" || ext == "cc")
                ext_type = ext_t::CXX;
            else
                ext_type = ext_t::Other;
            return input_t{name, ext, ext_type, std::stoi(id)};
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

    std::string time_message(const std::chrono::milliseconds &duration) {
        return std::to_string(duration.count());
    }
}
