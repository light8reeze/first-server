#include "Thread.h"

namespace first {
    
    void Thread::initialize() {
        is_stop_ = false;
    }

    void Thread::start() {
        thread_ = std::thread(run, this);
    }

    void Thread::wait() {
        if (thread_.joinable())
            thread_.join();
    }

    void Thread::stop() {
        is_stop_ = true;
    }

    void Thread::run() {
        while (!is_stop_) {
            routine();

            if(0 < tick_interval_)
                std::this_thread::sleep_for(std::chrono::milliseconds(tick_interval_));
        }
    }
}