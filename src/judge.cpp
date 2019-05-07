#include "util.h"
#include "judge.h"
#include "status.h"
#include "path.h"

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
#include <iostream>

namespace judge {
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
        Path source_path(source, offset);
        try {
            if (!std::filesystem::exists(source_path.get_file_name()))
                throw Status(status_t::NF);
            if (source.ext_type == ext_t::Other)
                throw Status(status_t::TE);
            compile(source_path);
            if (!std::filesystem::exists(source_path.get_exec_name()))
                throw Status(status_t::CE);
            execute(source_path);
            bool compare_result = compare(source_path);
            if (get_status() == status_t::LE || get_status() == status_t::RE)
                throw Status(get_status());
            throw (compare_result) ? Status(status_t::AC) : Status(status_t::WA);
        } catch (Status &status) {
            this->status = status.get_status();
            std::filesystem::remove(source_path.get_exec_path());
            std::filesystem::remove(source_path.get_res_path());
            std::filesystem::remove(source_path.get_log_path());
        }
    }

    void Judge::compile(const Path &source_path) {
        const std::filesystem::path &log_path = source_path.get_log_path();
        const std::filesystem::path &file_name = source_path.get_file_name();
        const std::filesystem::path &exec_name = source_path.get_exec_name();
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
        rusage usage{};
        int poll_count = 0;
        poll:
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_INTERVAL_MS));
        ++poll_count;
        if (!wait4(proc_compile, proc_status, WNOHANG, &usage)) {
            if (poll_count <= TIME_LIMIT_MS / TIME_INTERVAL_MS) {
                goto poll;
            } else {
                kill(proc_compile, SIGKILL);
                status = status_t::CE;
                return;
            }
        } else {
            compilation_time = time_cast(usage.ru_utime);
        }
    }

    void Judge::execute(const Path &source_path) {
        const std::filesystem::path &in_path = source_path.get_in_path();
        const std::filesystem::path &result_path = source_path.get_res_path();
        const std::filesystem::path &log_path = source_path.get_log_path();
        const std::filesystem::path &exec_path = source_path.get_exec_path();
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
        rusage usage{};
        int poll_count = 0;
        poll:
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_INTERVAL_MS));
        ++poll_count;
        if (!wait4(proc_execute, proc_status, WNOHANG, &usage)) {
            if (poll_count <= TIME_LIMIT_MS / TIME_INTERVAL_MS) {
                goto poll;
            } else {
                kill(proc_execute, SIGKILL);
                status = status_t::LE;
                return;
            }
        } else {
            execution_time = time_cast(usage.ru_utime);
            execution_memory = usage.ru_maxrss;
        }
    }

    bool Judge::compare(const Path &source_path) {
        std::ifstream out(source_path.get_out_path());
        std::ifstream result(source_path.get_res_path());
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
