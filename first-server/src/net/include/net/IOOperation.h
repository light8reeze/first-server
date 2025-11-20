#pragma once
#include <liburing.h>
#include <memory>

namespace first {
    class IOUringObject;
    class IOOperation {
    public:
        IOOperation(IOUringObject* io_object);
        virtual ~IOOperation() = default;


    public:
        virtual void request_io(io_uring* ring) = 0;
        virtual void handle_io_completion(int result) = 0;


    protected:
        std::shared_ptr<IOUringObject> io_object_;
    };
}