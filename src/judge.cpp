#include "judgement.h"
#include "judge.h"
#include "status.h"

#include <utility>
#include <filesystem>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>

namespace judgement {
    Judge::Judge(source_t source, const status_t &status) : source(std::move(source)), status(status),
                                                            compilation_time(0), execution_time(0),
                                                            execution_memory(0) {}


    const status_t &Judge::get_status() const {
        return status;
    }

    const std::chrono::milliseconds &Judge::get_compilation_time() const {
        return compilation_time;
    }

    const std::chrono::milliseconds &Judge::get_execution_time() const {
        return execution_time;
    }

    long Judge::get_execution_memory() const {
        return execution_memory;
    }

    void Judge::run(double offset) {
        const std::filesystem::path io_name = source.io_path;
        std::filesystem::path file_name = source.code_path;
        if (std::floor(offset)) {
            file_name += "_" + std::to_string((int) std::floor(offset)) + "." + source.ext;
            if (!std::filesystem::exists(file_name))
                file_name = source.code_path + "." + source.ext;
        } else {
            file_name += "." + source.ext;
        }
        const std::filesystem::path exec_name = file_name.parent_path().string() + "/" + std::to_string(offset);
        const std::filesystem::path exec_path = "./" + exec_name.string();
        const std::filesystem::path in_path = source.io_path + ".in";
        const std::filesystem::path out_path = source.io_path + ".out";
        const std::filesystem::path res_path = io_name.parent_path().string() + "/" + std::to_string(offset) + ".txt";
        const std::filesystem::path log_path = io_name.parent_path().string() + "/" + std::to_string(offset) + ".log";
        bool compare_result;
        try {
            if (!std::filesystem::exists(file_name))
                throw Status(status_t::NF);
            if (source.ext_type == ext_t::Other)
                throw Status(status_t::TE);
            compile(file_name, exec_name, log_path);
            if (!std::filesystem::exists(exec_name))
                throw Status(status_t::CE);
            execute(exec_path, res_path, log_path, in_path);
            compare_result = compare(out_path, res_path);
            if (get_status() == status_t::LE || get_status() == status_t::RE)
                throw Status(get_status());
            throw (compare_result) ? Status(status_t::AC) : Status(status_t::WA);
        } catch (Status &status) {
            this->status = status.get_status();
            std::filesystem::remove(exec_path);
            std::filesystem::remove(res_path);
            std::filesystem::remove(log_path);
        }
    }

    void Judge::compile(const std::filesystem::path &file_name, const std::filesystem::path &exec_name,
                        const std::filesystem::path &log_path) {
        int *proc_status = nullptr;
        pid_t proc_compile = fork();
        if (!proc_compile) {
            int fd = open(log_path.c_str(), O_RDWR | O_CREAT, 0644);
            dup2(fd, 2);
            close(fd);
            switch (source.ext_type) {
                case ext_t::C:
                    execlp(C_COMPILER, C_COMPILER, file_name.c_str(), SECCOMP_SOURCE, DL, SECCOMP, "-o",
                           exec_name.c_str(), nullptr);
                    break;
                case ext_t::CXX:
                    execlp(CXX_COMPILER, CXX_COMPILER, file_name.c_str(), SECCOMP_SOURCE, DL, SECCOMP,
                           "-o", exec_name.c_str(), nullptr);
                    break;
                default:
                    exit(0);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(TIME_LIMIT));
        rusage usage{};
        if (!wait4(proc_compile, proc_status, WNOHANG, &usage)) {
            kill(proc_compile, SIGKILL);
            status = status_t::CE;
            return;
        } else {
            compilation_time = time_cast(usage.ru_utime);
        }
    }

    void Judge::execute(const std::filesystem::path &exec_path, const std::filesystem::path &result_path,
                        const std::filesystem::path &log_path, const std::filesystem::path &in_path) {
        int *proc_status = nullptr;
        pid_t proc_execute = fork();
        if (!proc_execute) {
            if (std::filesystem::exists(in_path)) {
                int fd_in = open(in_path.c_str(), O_RDONLY);
                dup2(fd_in, 0);
                close(fd_in);
            }
            int fd_result = open(result_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            dup2(fd_result, 1);
            close(fd_result);
            int fd_log = open(log_path.c_str(), O_RDWR | O_CREAT, 0644);
            dup2(fd_log, 2);
            close(fd_log);
            execlp(exec_path.c_str(), exec_path.c_str(), nullptr);
        }
        std::this_thread::sleep_for(std::chrono::seconds(TIME_LIMIT));
        rusage usage{};
        if (!wait4(proc_execute, proc_status, WNOHANG, &usage)) {
            kill(proc_execute, SIGKILL);
            status = status_t::LE;
            return;
        } else {
            execution_time = time_cast(usage.ru_utime);
            execution_memory = usage.ru_maxrss;
        }
    }

    bool Judge::compare(const std::filesystem::path &out_path, const std::filesystem::path &result_path) {
        std::ifstream out(out_path);
        std::ifstream result(result_path);
        if (result.peek() == std::ifstream::traits_type::eof() && status != status_t::LE) {
            status = status_t::RE;
            return false;
        }
        bool flag = false;
        if (out.is_open() && result.is_open()) {
            std::string line_out, line_result;
            while (getline(out, line_out) && getline(result, line_result)) {
                if (line_out.empty() || line_result.empty())
                    continue;
                flag |= line_out == line_result;
            }
            out.close();
            result.close();
        }
        return flag;
    }
}
