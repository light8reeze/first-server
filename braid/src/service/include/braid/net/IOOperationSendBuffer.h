#pragma once
#include <braid/net/IOOperation.h>
#include <braid/message/SendBuffer.h>
#include <memory>

namespace braid {

    class IOOperationSendBuffer : public IOOperation {
    public:
        IOOperationSendBuffer(std::shared_ptr<IOUringObject>&& io_object, ObjectPtr<SendBuffer> send_buffer);
        virtual ~IOOperationSendBuffer() = default;


    public:
        virtual void request_io(io_uring* ring) override;
        virtual void handle_io_completion(int result) override;


    private:
        ObjectPtr<SendBuffer> send_buffer_;
    };
}
