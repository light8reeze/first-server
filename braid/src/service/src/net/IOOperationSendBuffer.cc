#include <braid/net/IOOperationSendBuffer.h>
#include <braid/net/IOUringObject.h>

namespace braid {
    IOOperationSendBuffer::IOOperationSendBuffer(std::shared_ptr<IOUringObject>&& io_object, ObjectPtr<SendBuffer> send_buffer)
        : IOOperation(std::move(io_object)), send_buffer_(send_buffer) {
    }

    void IOOperationSendBuffer::request_io(io_uring* ring) {
        io_uring_sqe* sqe = ::io_uring_get_sqe(ring);

        ::io_uring_prep_send(sqe, io_object_->get_socket_fd(), send_buffer_->get_buffer().data(), send_buffer_->get_buffer().size(), 0);
        ::io_uring_sqe_set_data(sqe, this);
    }

    void IOOperationSendBuffer::handle_io_completion(int result) {
        if (nullptr == io_object_)
            return;

        if (result < 0)
            return;

        io_object_->on_sent(result);
    }
}