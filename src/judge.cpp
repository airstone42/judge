#include "judge.h"

#include <utility>
#include <chrono>
#include <string>
#include <filesystem>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>

namespace judgement {
    Judge::Judge(source_t source, const status_t &status) : source(std::move(source)), status(status),
                                                            compiling_pid(-1), executing_pid(-1),
                                                            compiling_time(0),
                                                            executing_time(0) {}


    const status_t &Judge::get_status() const {
        return this->status;
    }

    const std::chrono::milliseconds &Judge::get_compiling_time() const {
        return this->compiling_time;
    }

    const std::chrono::milliseconds &Judge::get_executing_time() const {
        return this->executing_time;
    }

    void Judge::run() {
        const std::filesystem::path filename = this->source.name + "." + this->source.ext;
        const std::filesystem::path exec_path = "./" + this->source.name;
        const std::filesystem::path in_path = this->source.name + ".in";
        const std::filesystem::path out_path = this->source.name + ".out";
        const std::filesystem::path err_path = this->source.name + ".err";
        if (!std::filesystem::exists(filename)) {
            this->status = status_t::FileNotFound;
            return;
        }
        if (this->source.ext_type == ext_t::Other) {
            this->status = status_t::SourceTypeError;
            return;
        }
        compile(filename, source.name, err_path);
        if (!std::filesystem::exists(this->source.name))
            this->status = status_t::CompileError;
        if (this->get_status() == status_t::CompileError)
            return;
        execute(exec_path, out_path, err_path);
        if (this->get_status() == status_t::LimitExceed || this->get_status() == status_t::RuntimeError)
            return;
        this->status = (!compare(in_path, out_path)) ? status_t::Accepted : status_t::WrongAnswer;
        std::filesystem::remove(exec_path);
    }

    void Judge::compile(const std::filesystem::path &filename, const std::filesystem::path &exec_name,
                        const std::filesystem::path &err_path) {
        int *proc_status = nullptr;
        pid_t proc_compile = fork();
        if (!proc_compile) {
            int fd = open(err_path.c_str(), O_RDWR | O_CREAT);
            dup2(fd, 2);
            close(fd);
            switch (this->source.ext_type) {
                case ext_t::C:
                    execlp(C_COMPILER, C_COMPILER, filename.c_str(), "-o", exec_name.c_str(), nullptr);
                    break;
                case ext_t::CXX:
                    execlp(CXX_COMPILER, CXX_COMPILER, filename.c_str(), "-o", exec_name.c_str(), nullptr);
                    break;
                default:
                    exit(0);
            }
        }
        this->compiling_pid = proc_compile;
        sleep(TIME_LIMIT);
        if (!waitpid(proc_compile, proc_status, WNOHANG)) {
            kill(proc_compile, SIGKILL);
            this->status = status_t::CompileError;
            return;
        } else {
            rusage usage{};
            getrusage(RUSAGE_CHILDREN, &usage);
            this->compiling_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::seconds(usage.ru_utime.tv_sec) + std::chrono::microseconds(usage.ru_utime.tv_usec));
        }
    }

    void Judge::execute(const std::filesystem::path &exec_path, const std::filesystem::path &out_path,
                        const std::filesystem::path &err_path) {
        int *proc_status = nullptr;
        pid_t proc_execute = fork();
        if (!proc_execute) {
            int fd_out = open(out_path.c_str(), O_RDWR | O_CREAT | O_TRUNC);
            int fd_err = open(err_path.c_str(), O_RDWR | O_CREAT);
            dup2(fd_out, 1);
            dup2(fd_err, 2);
            close(fd_out);
            close(fd_err);
            execlp(exec_path.c_str(), exec_path.c_str(), nullptr);
        }
        this->executing_pid = proc_execute;
        sleep(TIME_LIMIT);
        if (!waitpid(proc_execute, proc_status, WNOHANG)) {
            kill(proc_execute, SIGKILL);
            this->status = status_t::LimitExceed;
            return;
        } else {
            rusage usage{};
            getrusage(RUSAGE_CHILDREN, &usage);
            this->executing_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::seconds(usage.ru_utime.tv_sec) + std::chrono::microseconds(usage.ru_utime.tv_usec) -
                    this->compiling_time);
        }
    }

    bool Judge::compare(const std::filesystem::path &in_path, const std::filesystem::path &out_path) {
        bool flag = false;
        std::ifstream in(in_path);
        std::ifstream out(out_path);
        if (in.is_open() && out.is_open()) {
            std::string line_in, line_out;
            while (getline(in, line_in) && getline(out, line_out))
                flag |= line_in.compare(line_out);
            in.close();
            out.close();
        }
        return flag;
    }
}
