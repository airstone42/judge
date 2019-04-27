#ifndef JUDGE_H
#define JUDGE_H

#include "judgement.h"
#include "source_path.h"

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

        void compile(const SourcePath &source_path);

        void execute(const SourcePath &source_path);

        bool compare(const SourcePath &source_path);

    private:
        source_t source;
        status_t status;
        std::chrono::milliseconds compilation_time;
        std::chrono::milliseconds execution_time;
        long execution_memory;
    };
}

#endif
