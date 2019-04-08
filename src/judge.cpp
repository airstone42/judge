#include "judge.h"

#include <utility>
#include <chrono>
#include <string>
#include <cmath>
#include <filesystem>
#include <thread>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>

namespace judgement {
    Judge::Judge(source_t source, const status_t &status) : source(std::move(source)), status(status),
                                                            compiling_pid(-1), executing_pid(-1), compiling_time(0),
                                                            executing_time(0), executing_memory(0) {}


    const status_t &Judge::get_status() const {
        return this->status;
    }

    const std::chrono::milliseconds &Judge::get_compiling_time() const {
        return this->compiling_time;
    }

    const std::chrono::milliseconds &Judge::get_executing_time() const {
        return this->executing_time;
    }

    long Judge::get_executing_memory() const {
        return executing_memory;
    }

    void Judge::run(double offset) {
        const std::filesystem::path io_name = this->source.name;
        std::filesystem::path file_name = this->source.name;
        if (std::floor(offset)) {
            file_name += "_" + std::to_string((int) std::floor(offset)) + "." + this->source.ext;
            if (!std::filesystem::exists(file_name))
                file_name = this->source.name + "." + this->source.ext;
        } else {
            file_name += "." + this->source.ext;
        }
        const std::filesystem::path exec_name = file_name.parent_path().string() + "/" + std::to_string(offset);
        const std::filesystem::path exec_path = "./" + exec_name.string();
        const std::filesystem::path in_path = this->source.name + ".in";
        const std::filesystem::path out_path = this->source.name + ".out";
        const std::filesystem::path result_path =
                file_name.parent_path().string() + "/" + std::to_string(offset) + ".txt";
        const std::filesystem::path log_path = file_name.parent_path().string() + "/" + std::to_string(offset) + ".log";
        bool compare_result = false;
        if (!std::filesystem::exists(file_name)) {
            this->status = status_t::NF;
            goto remove;
        }
        if (this->source.ext_type == ext_t::Other) {
            this->status = status_t::TE;
            goto remove;
        }
        compile(file_name, exec_name, log_path);
        if (!std::filesystem::exists(exec_name))
            this->status = status_t::CE;
        if (this->get_status() == status_t::CE)
            goto remove;
        execute(exec_path, result_path, log_path, in_path);
        compare_result = compare(out_path, result_path);
        if (this->get_status() == status_t::LE || this->get_status() == status_t::RE)
            goto remove;
        this->status = (compare_result) ? status_t::AC : status_t::WA;
        remove:
        std::filesystem::remove(exec_path);
        std::filesystem::remove(result_path);
        std::filesystem::remove(log_path);
    }

    void Judge::compile(const std::filesystem::path &file_name, const std::filesystem::path &exec_name,
                        const std::filesystem::path &log_path) {
        int *proc_status = nullptr;
        pid_t proc_compile = fork();
        if (!proc_compile) {
            int fd = open(log_path.c_str(), O_RDWR | O_CREAT, 0644);
            dup2(fd, 2);
            close(fd);
            switch (this->source.ext_type) {
                case ext_t::C:
                    execlp(C_COMPILER, C_COMPILER, file_name.c_str(), SECCOMP_SOURCE, DL_LIBRARY, SECCOMP_LIBRARY, "-o",
                           exec_name.c_str(), nullptr);
                    break;
                case ext_t::CXX:
                    execlp(CXX_COMPILER, CXX_COMPILER, file_name.c_str(), SECCOMP_SOURCE, DL_LIBRARY, SECCOMP_LIBRARY,
                           "-o", exec_name.c_str(), nullptr);
                    break;
                default:
                    exit(0);
            }
        }
        this->compiling_pid = proc_compile;
        rusage usage{};
        std::this_thread::sleep_for(std::chrono::seconds(TIME_LIMIT));
        if (!wait4(proc_compile, proc_status, WNOHANG, &usage)) {
            kill(proc_compile, SIGKILL);
            this->status = status_t::CE;
            return;
        } else {
            this->compiling_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::seconds(usage.ru_utime.tv_sec) + std::chrono::microseconds(usage.ru_utime.tv_usec));
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
        this->executing_pid = proc_execute;
        rusage usage{};
        std::this_thread::sleep_for(std::chrono::seconds(TIME_LIMIT));
        if (!wait4(proc_execute, proc_status, WNOHANG, &usage)) {
            kill(proc_execute, SIGKILL);
            this->status = status_t::LE;
            return;
        } else {
            this->executing_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::seconds(usage.ru_utime.tv_sec) + std::chrono::microseconds(usage.ru_utime.tv_usec));
            this->executing_memory = usage.ru_maxrss;
        }
    }

    bool Judge::compare(const std::filesystem::path &out_path, const std::filesystem::path &result_path) {
        std::ifstream out(out_path);
        std::ifstream result(result_path);
        if (result.peek() == std::ifstream::traits_type::eof() && this->status != status_t::LE) {
            this->status = status_t::RE;
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
