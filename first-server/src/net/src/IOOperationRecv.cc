#include <liburing.h>
#include <net/IOOperationRecv.h>
#include <net/IOUringObject.h>

namespace first {
    IOOperationRecv::IOOperationRecv(IOUringObject* io_object)
        : IOOperation(io_object) {
    }

    void IOOperationRecv::request_io(io_uring* ring) {
		io_uring_sqe* sqe = io_uring_get_sqe(ring);
        
		::io_uring_prep_recv(sqe, io_object_->get_socket_fd(), io_object_->get_buffer(), io_object_->get_buffer_size(), 0);
		::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationRecv::handle_io_completion(int result) {
        if(nullptr == io_object_) {
            return; 
        }

        if(result < 0) {
            // TODO: Handle error
            return;
        }

        io_object_->on_received(result);
    }
}