#pragma once
#include <util/Macro.h>

struct io_uring;
struct io_uring_cqe;

namespace braid {	
	class IOOperation;
	class IOCompletion {
		NON_COPYABLE(IOCompletion);

	public:
		IOCompletion(io_uring* ring, io_uring_cqe* cqe);
		IOCompletion(IOCompletion&& io_completion) noexcept;
		~IOCompletion();


	public:
		void handle_completion();


	public:
		int get_result() const;
		IOOperation* get_completed_operation() const { return completed_operation_; }
		

	private:
		io_uring*		ring_ = nullptr;
		io_uring_cqe*	cqe_ = nullptr;
		IOOperation*	completed_operation_ = nullptr;
	};
}