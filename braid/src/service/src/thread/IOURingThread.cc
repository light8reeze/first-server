#include <thread/IOURingThread.h>
#include <liburing.h>

namespace first {

    IOURingThread::IOURingThread(int queue_depth /* = 1024 */) 
        : Thread(), ring_queue_(queue_depth) {
    }

    IOURingThread::~IOURingThread() {
    }

    void IOURingThread::request_io(IOOperation* operation) {
        ring_queue_.push_request(operation);
    }
}