#include <net/IOOperationNop.h>
#include <net/IOUringObject.h>
#include <liburing.h>

namespace first {
    IOOperationNop::IOOperationNop(std::shared_ptr<IOUringObject>&& io_object)
        : IOOperation(std::move(io_object)) {
    }

    void IOOperationNop::request_io(io_uring* ring) {

        if(nullptr == io_object_)
            return;

        if(nullptr == ring)
            return;
            
        on_event_request();

        io_uring_sqe* sqe = io_uring_get_sqe(ring);
        ::io_uring_prep_nop(sqe);
        ::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationNop::handle_io_completion(int result) {
        if(nullptr == io_object_)
            return;

        if(result < 0)
            return;

        on_event_completed();
    }
}