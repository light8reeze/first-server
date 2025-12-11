#include <liburing.h>
#include <net/IOOperationSend.h>
#include <net/IOUringObject.h>

namespace first {
    IOOperationSend::IOOperationSend(std::shared_ptr<IOUringObject>&& io_object)
        : IOOperation(std::move(io_object)) {
    }

    void IOOperationSend::request_io(io_uring* ring) {
        io_uring_sqe* sqe = ::io_uring_get_sqe(ring);

        ::io_uring_prep_send(sqe, io_object_->get_socket_fd(), io_object_->get_buffer(), io_object_->get_buffer_size(), 0);
        ::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationSend::handle_io_completion(int result) {
        if (nullptr == io_object_)
            return;

        if (result < 0)
            return;

        io_object_->on_sent(result);
    }

}