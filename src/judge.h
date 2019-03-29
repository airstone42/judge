#ifndef JUDGE_H
#define JUDGE_H

#include "judgement.h"

#include <string>

namespace judgement {
    class Judge {
    public:
        explicit Judge(input_t input, const status_t &status = status_t::Waiting);

        const status_t &get_status() const;

        void run();

        void compile(const std::string &filename, const std::string &exec_name, const std::string &err_path);

        void execute(const std::string &exec_path, const std::string &out_path, const std::string &err_path);

        bool compare(const std::string &in_path, const std::string &out_path);

    private:
        input_t input;
        status_t status;
    };
}


#endif
