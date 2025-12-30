#include <braid/net/IORequestQueue.h>
#include <braid/net/IOUringObject.h>
#include <braid/net/IOOperation.h>
#include <braid/util/LogHandler.h>

#include <cassert>

namespace braid {

	IORequestQueue::IORequestQueue(int queue_depth /*= 1024*/) 
		: request_queue_(queue_depth) {
		::io_uring_queue_init(queue_depth, &ring_, 0);
	}

	IORequestQueue::~IORequestQueue() {
		::io_uring_queue_exit(&ring_);
	}

	IOCompletion IORequestQueue::wait_one(int timeout_ms /*= -1*/) {
		struct __kernel_timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = timeout_ms * 1000000;

		io_uring_cqe* cqe;
		int ret = 0;
		if (0 <= timeout_ms)
			ret = ::io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
		else
			ret = ::io_uring_wait_cqe(&ring_, &cqe);

		if (ret < 0) {
			switch(ret) {
				case -EINTR:
					[[fallthrough]]
				case -ETIME:
					return IOCompletion(nullptr, nullptr);
				default:
					LOG_ERROR("io_uring_wait_cqe_timeout failed: %d", ret);
					return IOCompletion(ret);
			}
		}

		return IOCompletion(&ring_, cqe);
	}

	void IORequestQueue::submit() {
		::io_uring_submit(&ring_);
	}

	void IORequestQueue::push_request(IOOperation* operation) {
		assert(nullptr != operation);
		int count = 0;
		while (!request_queue_.push(operation) && count++ < 10) {
			// TODO: push 회수 제한, 예외처리
		}
	}

	void IORequestQueue::flush_requests() {
		if (request_queue_.empty())
			return;

		IOOperation* operation = nullptr;
		while (request_queue_.pop(operation)) {
			assert(nullptr != operation);

			if (operation)
				operation->request_io(&ring_);
		}

		submit();
	}
}