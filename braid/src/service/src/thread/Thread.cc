#include <thread/Thread.h>

namespace first {
    
    void Thread::initialize() {
        is_stop_ = false;
    }

    void Thread::start() {
		auto thread_func = [this]() {
			this->run();
		};

        thread_ = std::thread(thread_func);
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