#include <net/IOCompletion.h>
#include <net/IOOperation.h>
#include <liburing.h>

namespace first {
	IOCompletion::IOCompletion(io_uring* ring, io_uring_cqe* cqe)
		: ring_(ring), cqe_(cqe) {
		if(nullptr != cqe_)
			completed_operation_ = reinterpret_cast<IOOperation*>(::io_uring_cqe_get_data(cqe_));
	}

	IOCompletion::IOCompletion(IOCompletion&& io_completion) noexcept
		: ring_(io_completion.ring_), cqe_(io_completion.cqe_) {
		io_completion.ring_ = nullptr;
		io_completion.cqe_	= nullptr;
	}
	
	IOCompletion::~IOCompletion() {
		if (nullptr != ring_ && nullptr != cqe_)
			::io_uring_cqe_seen(ring_, cqe_);
	}

	int IOCompletion::get_result() const {
		return cqe_ ? cqe_->res : -1;
	}

	void IOCompletion::handle_completion() {
		if (completed_operation_)
			completed_operation_->handle_io_completion(get_result());
	}
}