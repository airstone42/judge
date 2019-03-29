#include "judge.h"

#include <utility>
#include <string>
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

namespace judgement {
    Judge::Judge(input_t input, const status_t &status) : input(std::move(input)), status(status) {}

    const status_t &Judge::get_status() const {
        return this->status;
    }

    void Judge::run() {
        const std::string filename = this->input.name + "." + this->input.ext;
        const std::string exec_path = "./" + this->input.name;
        const std::string in_path = this->input.name + ".in";
        const std::string out_path = this->input.name + ".out";
        const std::string err_path = this->input.name + ".err";
        if (access(filename.c_str(), R_OK)) {
            this->status = status_t::FileNotFound;
            return;
        }
        if (this->input.source_type == source_t::Other) {
            this->status = status_t::SourceTypeError;
            return;
        }
        compile(filename, input.name, err_path);
        if (access(this->input.name.c_str(), R_OK)) {
            this->status = status_t::CompileError;
            return;
        }
        execute(exec_path, out_path, err_path);
        this->status = (!compare(in_path, out_path)) ? status_t::Accepted : status_t::WrongAnswer;
        remove(exec_path.c_str());
    }

    void Judge::compile(const std::string &filename, const std::string &exec_name, const std::string &err_path) {
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
    }

    void Judge::execute(const std::string &exec_path, const std::string &out_path, const std::string &err_path) {
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
    }

    bool Judge::compare(const std::string &in_path, const std::string &out_path) {
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
