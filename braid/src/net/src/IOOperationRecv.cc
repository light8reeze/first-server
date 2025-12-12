#include <liburing.h>
#include <assert.h>
#include <braid/net/IOOperationRecv.h>
#include <braid/net/IOUringObject.h>

namespace braid {
    IOOperationRecv::IOOperationRecv(std::shared_ptr<IOUringObject>&& io_object)
        : IOOperation(std::move(io_object)) {
    }

    void IOOperationRecv::request_io(io_uring* ring) {
		assert(nullptr != io_object_);

		io_uring_sqe* sqe = io_uring_get_sqe(ring);
        
        std::span<char> span = io_object_->get_remain_span();
		::io_uring_prep_recv(sqe, io_object_->get_socket_fd(), span.data(), span.size(), 0);
		::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationRecv::handle_io_completion(int result) {
        assert(nullptr != io_object_);

        if(result < 0) {
            io_object_->on_receive_failed(result);
            return;
        }

        io_object_->commit(result);
        io_object_->on_received(result);
    }
}