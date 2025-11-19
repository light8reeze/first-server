#include "IOOperationAccept.h"
#include <liburing.h>
#include "IOUringObject.h"

namespace first {
    IOOperationAccept::IOOperationAccept(IOUringObject* io_object)
        : IOOperation(io_object) {
    }

    void IOOperationAccept::request_io(io_uring* ring) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
        
        socklen_t addrlen = sizeof(io_object_->get_address());
        struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&io_object_->get_address());

        ::io_uring_prep_accept(sqe, io_object_->get_socket_fd(), addr, &addrlen, 0);
        ::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationAccept::handle_io_completion(int result) {
        if(nullptr == io_object_) {
            return; 
        }

        if(result < 0) {
     
        }
    }