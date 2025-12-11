#pragma once
#include <net/IOOperation.h>

struct io_uring;

namespace braid {
    class IOOperationRecv : public IOOperation {
    public:
        IOOperationRecv(std::shared_ptr<IOUringObject>&& io_object);
        virtual ~IOOperationRecv() = default;

        
    public:
        virtual void request_io(io_uring* ring) override;
        virtual void handle_io_completion(int result) override;
    };
}