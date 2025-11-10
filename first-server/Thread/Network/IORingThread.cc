#include "IORingThread.h"
#include <liburing.h>

namespace first {

    IORingThread::IORingThread(int queue_depth /* = 1024 */) {
        io_uring_queue_init(queue_depth, &ring_, 0);
    }

    IORingThread::~IORingThread() {
        io_uring_queue_exit(&ring_);
    }

    io_uring* IORingThread::getRing() {
        return &ring_;
    }
}