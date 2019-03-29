#ifndef JUDGE_H
#define JUDGE_H

#include "judgement.h"

#include <filesystem>

namespace judgement {
    class Judge {
    public:
        explicit Judge(input_t input, const status_t &status = status_t::Waiting);

        const status_t &get_status() const;

        void run();

        void compile(const std::filesystem::path &filename, const std::filesystem::path &exec_name,
                     const std::filesystem::path &err_path);

        void execute(const std::filesystem::path &exec_path, const std::filesystem::path &out_path,
                     const std::filesystem::path &err_path);

        bool compare(const std::filesystem::path &in_path, const std::filesystem::path &out_path);

    private:
        input_t input;
        status_t status;
    };
}


#endif
