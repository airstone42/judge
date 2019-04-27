#ifndef JUDGE_H
#define JUDGE_H

#include "judgement.h"

#include <filesystem>
#include <chrono>

namespace judgement {
    class Judge {
    public:
        explicit Judge(source_t source, const status_t &status = status_t::W);

        const status_t &get_status() const;

        const std::chrono::milliseconds &get_compilation_time() const;

        const std::chrono::milliseconds &get_execution_time() const;

        long get_execution_memory() const;

        void run(double offset);

        void compile(const std::filesystem::path &file_name, const std::filesystem::path &exec_name,
                     const std::filesystem::path &log_path);

        void execute(const std::filesystem::path &exec_path, const std::filesystem::path &result_path,
                     const std::filesystem::path &log_path,
                     const std::filesystem::path &in_path);

        bool compare(const std::filesystem::path &out_path, const std::filesystem::path &result_path);

    private:
        source_t source;
        status_t status;
        std::chrono::milliseconds compilation_time;
        std::chrono::milliseconds execution_time;
        long execution_memory;
    };
}

#endif
