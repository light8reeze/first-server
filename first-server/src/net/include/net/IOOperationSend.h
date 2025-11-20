#pragma once
#include <net/IOOperation.h>

struct io_uring;

namespace first {
    class IOOperationSend : public IOOperation {
    public:
        IOOperationSend(IOUringObject* io_object);
        virtual ~IOOperationSend() = default;

        
    public:
        virtual void request_io(io_uring* ring) override;
        virtual void handle_io_completion(int result) override;
    };
}