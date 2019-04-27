#include "source_path.h"

#include <string>
#include <cmath>

namespace judgement {
    SourcePath::SourcePath(const source_t &source, double offset) : SourcePath(source.io_path, source.code_path,
                                                                               source.ext, offset) {}

    SourcePath::SourcePath(const std::string &io_path, const std::string &code_path, const std::string &ext,
                           double offset) : file_name(std::floor(offset) ? !std::filesystem::exists(file_name) ?
                                                                           code_path + "_" +
                                                                           std::to_string((int) std::floor(offset)) +
                                                                           "." + ext : code_path + "." + ext :
                                                      code_path + "." + ext),
                                            in_path(io_path + ".in"), out_path(io_path + ".out"),
                                            exec_name(file_name.parent_path().string() + "/" + std::to_string(offset)),
                                            exec_path("./" + exec_name.string()),
                                            res_path(std::filesystem::path(io_path).parent_path().string() + "/" +
                                                     std::to_string(offset) + ".txt"),
                                            log_path(std::filesystem::path(io_path).parent_path().string() + "/" +
                                                     std::to_string(offset) + ".log") {}

    const std::filesystem::path &SourcePath::get_file_name() const {
        return file_name;
    }

    const std::filesystem::path &SourcePath::get_exec_name() const {
        return exec_name;
    }

    const std::filesystem::path &SourcePath::get_exec_path() const {
        return exec_path;
    }

    const std::filesystem::path &SourcePath::get_in_path() const {
        return in_path;
    }

    const std::filesystem::path &SourcePath::get_out_path() const {
        return out_path;
    }

    const std::filesystem::path &SourcePath::get_res_path() const {
        return res_path;
    }

    const std::filesystem::path &SourcePath::get_log_path() const {
        return log_path;
    }
}