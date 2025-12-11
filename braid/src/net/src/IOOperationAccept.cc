#include <net/IOOperationAccept.h>
#include <net/IOUringObject.h>
#include <liburing.h>
#include <cassert>


namespace first {
    IOOperationAccept::IOOperationAccept(std::shared_ptr<IOUringObject>&& io_object, int listen_fd)
        : listen_fd_(listen_fd), IOOperation(std::move(io_object)) {
    }

    void IOOperationAccept::request_io(io_uring* ring) {
        io_uring_sqe* sqe = io_uring_get_sqe(ring);

        socklen_t addrlen = sizeof(io_object_->get_address());
        struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&io_object_->get_address());

        ::io_uring_prep_accept(sqe, listen_fd_, addr, &addrlen, 0);
        ::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationAccept::handle_io_completion(int result) {
        assert(nullptr != io_object_);
        
        if (nullptr == io_object_) {
            return;
        }

        if (result < 0) {
            io_object_->on_accept_failed(result);
            return;
        }

        int client_fd = result;
        io_object_->set_socket_fd(client_fd);
        io_object_->on_accepted();
    }
}