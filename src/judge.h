#ifndef JUDGE_H
#define JUDGE_H

#include "judgement.h"

#include <filesystem>
#include <chrono>

#include <sys/types.h>

namespace judgement {
    class Judge {
    public:
        explicit Judge(source_t source, const status_t &status = status_t::Waiting);

        void set_status(status_t status);

        const status_t &get_status() const;

        pid_t get_compiling_pid() const;

        void set_compiling_pid(pid_t compilingPid);

        pid_t get_executing_pid() const;

        void set_executing_pid(pid_t executingPid);

        const std::chrono::milliseconds &get_compiling_time() const;

        void set_compiling_time(const std::chrono::milliseconds &time);

        const std::chrono::milliseconds &get_executing_time() const;

        void set_executing_time(const std::chrono::milliseconds &time);

        void run();

        void compile(const std::filesystem::path &filename, const std::filesystem::path &exec_name,
                     const std::filesystem::path &err_path);

        void execute(const std::filesystem::path &exec_path, const std::filesystem::path &out_path,
                     const std::filesystem::path &err_path);

        bool compare(const std::filesystem::path &in_path, const std::filesystem::path &out_path);

    private:
        source_t source;
        status_t status;
        pid_t compiling_pid;
        pid_t executing_pid;
        std::chrono::milliseconds compiling_time;
        std::chrono::milliseconds executing_time;
    };
}


#endif
