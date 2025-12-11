#pragma once
#include <net/IOOperation.h>

struct io_uring;

namespace braid {
    class IOOperationAccept : public IOOperation {
    public:
        IOOperationAccept(std::shared_ptr<IOUringObject>&& io_object, int listen_fd);
        virtual ~IOOperationAccept() = default;


    public:
        virtual void request_io(io_uring* ring) override;
        virtual void handle_io_completion(int result) override;


    private:
        int listen_fd_ = -1;
    };
}