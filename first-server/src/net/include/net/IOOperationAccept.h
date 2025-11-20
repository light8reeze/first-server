#pragma once
#include <net/IOOperation.h>

struct io_uring;

namespace first {
    class IOOperationAccept : public IOOperation {
    public:
        IOOperationAccept(IOUringObject* io_object);
        virtual ~IOOperationAccept() = default;


    public:
        virtual void request_io(io_uring* ring) override;
        virtual void handle_io_completion(int result) override;
    };
}