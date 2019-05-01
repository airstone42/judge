#include "status.h"

namespace judge {
    Status::Status(status_t status) : status(status) {}

    status_t Status::get_status() const {
        return status;
    }
}
