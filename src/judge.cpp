#include "judge.h"

#include <utility>
#include <chrono>
#include <string>
#include <filesystem>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

namespace judgement {
    Judge::Judge(input_t input, const status_t &status) : input(std::move(input)), status(status), compiling_time(0),
                                                          running_time(0) {}

    const status_t &Judge::get_status() const {
        return this->status;
    }

    const std::chrono::milliseconds &Judge::get_compiling_time() const {
        return this->compiling_time;
    }

    void Judge::set_compiling_time(const std::chrono::milliseconds &time) {
        this->compiling_time = time;
    }

    const std::chrono::milliseconds &Judge::get_running_time() const {
        return this->running_time;
    }

    void Judge::set_running_time(const std::chrono::milliseconds &time) {
        this->running_time = time;
    }

    void Judge::run() {
        const std::filesystem::path filename = this->input.name + "." + this->input.ext;
        const std::filesystem::path exec_path = "./" + this->input.name;
        const std::filesystem::path in_path = this->input.name + ".in";
        const std::filesystem::path out_path = this->input.name + ".out";
        const std::filesystem::path err_path = this->input.name + ".err";
        if (!std::filesystem::exists(filename)) {
            this->status = status_t::FileNotFound;
            return;
        }
        if (this->input.source_type == source_t::Other) {
            this->status = status_t::SourceTypeError;
            return;
        }
        compile(filename, input.name, err_path);
        if (!std::filesystem::exists(this->input.name)) {
            this->status = status_t::CompileError;
            return;
        }
        execute(exec_path, out_path, err_path);
        this->status = (!compare(in_path, out_path)) ? status_t::Accepted : status_t::WrongAnswer;
        std::filesystem::remove(exec_path);
    }

    void Judge::compile(const std::filesystem::path &filename, const std::filesystem::path &exec_name,
                        const std::filesystem::path &err_path) {
        std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
        int *proc_status = nullptr;
        pid_t proc_compile = fork();
        if (!proc_compile) {
            int fd = open(err_path.c_str(), O_RDWR | O_CREAT);
            dup2(fd, 2);
            close(fd);
            switch (this->input.source_type) {
                case source_t::CSource:
                    execlp(C_COMPILER, C_COMPILER, filename.c_str(), "-o", exec_name.c_str(), nullptr);
                    break;
                case source_t::CXXSource:
                    execlp(CXX_COMPILER, CXX_COMPILER, filename.c_str(), "-o", exec_name.c_str(), nullptr);
                    break;
                default:
                    exit(0);
            }
        }
        waitpid(proc_compile, proc_status, 0);
        std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();
        this->set_compiling_time(std::chrono::duration_cast<std::chrono::milliseconds>(after - before));
    }

    void Judge::execute(const std::filesystem::path &exec_path, const std::filesystem::path &out_path,
                        const std::filesystem::path &err_path) {
        std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
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
        waitpid(proc_execute, proc_status, 0);
        std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();
        this->set_running_time(std::chrono::duration_cast<std::chrono::milliseconds>(after - before));
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
