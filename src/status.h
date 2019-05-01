#ifndef STATUS_H
#define STATUS_H

#include "util.h"

#include <exception>

namespace judge {
    class Status : public std::exception {
        friend std::string status_message(const status_t &status);

    public:
        explicit Status(status_t status);

        status_t get_status() const;

    private:
        status_t status;
    };
}

#endif
