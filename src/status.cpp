#include "status.h"

namespace judgement {
    Status::Status(status_t status) : status(status) {}

    status_t Status::get_status() const {
        return this->status;
    }
}
