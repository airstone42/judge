#ifndef SOURCE_PATH_H
#define SOURCE_PATH_H

#include "judgement.h"

#include <string>
#include <filesystem>

namespace judgement {
    class SourcePath {
    public:
        SourcePath(const source_t &source, double offset);

        SourcePath(const std::string &io_path, const std::string &code_path, const std::string &ext, double offset);

        const std::filesystem::path &get_file_name() const;

        const std::filesystem::path &get_exec_name() const;

        const std::filesystem::path &get_exec_path() const;

        const std::filesystem::path &get_in_path() const;

        const std::filesystem::path &get_out_path() const;

        const std::filesystem::path &get_res_path() const;

        const std::filesystem::path &get_log_path() const;

    private:
        std::filesystem::path file_name;
        std::filesystem::path exec_name;
        std::filesystem::path exec_path;
        std::filesystem::path in_path;
        std::filesystem::path out_path;
        std::filesystem::path res_path;
        std::filesystem::path log_path;
    };
}


#endif
