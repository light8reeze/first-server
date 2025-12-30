#include <braid/net/IOCompletion.h>

#include <liburing.h>

namespace braid {
	IOCompletion::IOCompletion(io_uring* ring, io_uring_cqe* cqe) noexcept
		: ring_(ring), cqe_(cqe) {	
		if(nullptr != cqe_) {
			IOOperation* op = reinterpret_cast<IOOperation*>(::io_uring_cqe_get_data(cqe_));
			completed_operation_ = std::move(ObjectPtr<IOOperation>(op, false));
			result_ = cqe_->res;
		}
	}

	IOCompletion::IOCompletion(IOCompletion&& io_completion) noexcept
		: ring_(io_completion.ring_), cqe_(io_completion.cqe_), result_(io_completion.result_) {
		io_completion.ring_ = nullptr;
		io_completion.cqe_	= nullptr;
		io_completion.result_ = 0;
	}

	IOCompletion::IOCompletion(int result) noexcept
		: ring_(nullptr), cqe_(nullptr), result_(result) {}
	
	IOCompletion::~IOCompletion() {

		if (nullptr != ring_ && nullptr != cqe_)
			::io_uring_cqe_seen(ring_, cqe_);
	}

	int IOCompletion::get_result() const {
		return result_;
	}

	void IOCompletion::handle_completion() {
		if (completed_operation_)
			completed_operation_->handle_io_completion(get_result());
	}
}