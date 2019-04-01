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

        const status_t &get_status() const;

        const std::chrono::milliseconds &get_compiling_time() const;

        const std::chrono::milliseconds &get_executing_time() const;

        long get_executing_memory() const;

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
        long executing_memory;
    };
}


#endif
