#include <memory>
#include <vector>
#include <assert.h>

#include <thread/IOThreadPool.h>
#include <thread/WorkerThread.h>

namespace first {
    static IOThreadPool* instance_ = nullptr;

    IOThreadPool::IOThreadPool(int num_threads /* = 4 */, int queue_depth /* = 1024 */)
        : num_threads_(num_threads), queue_depth_(queue_depth) {
        assert(nullptr == instance_);
        instance_ = this;
    }

    IOThreadPool::~IOThreadPool() {
        stop();
        worker_threads_.clear();
    }

    void IOThreadPool::initialize() {
        for (int i = 0; i < num_threads_; ++i) {
            worker_threads_.emplace_back(std::make_unique<WorkerThread>(queue_depth_));
            worker_threads_.back()->set_thread_id(i);
            worker_threads_.back()->initialize();
        }
    }

    void IOThreadPool::start() {
        for (auto& thread : worker_threads_)
            thread->start();
    }

    void IOThreadPool::stop() {
        for (auto& thread : worker_threads_)
            thread->stop();
    }

	void IOThreadPool::stop(int thread_index) {
		if (thread_index < 0 || thread_index >= num_threads_)
			return;

		worker_threads_[thread_index]->stop();
	}

	void IOThreadPool::wait_all()
	{
		for (auto& thread : worker_threads_)
			thread->wait();
	}

	void IOThreadPool::request_io(IOOperation* operation) {
        static std::atomic<int> request_index{ 0 };
        int thread_index = request_index++ % num_threads_;

        request_io(thread_index, operation);
    }

    void IOThreadPool::request_io(int thread_index, IOOperation* operation) {
        if (thread_index < 0 || thread_index >= num_threads_)
            return;

        worker_threads_[thread_index]->request_io(operation);
    }
}