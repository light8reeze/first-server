#pragma once
#include <braid/util/Macro.h>
#include <braid/net/IOOperation.h>

struct io_uring;
struct io_uring_cqe;

namespace braid {	
	class IOCompletion {
		NON_COPYABLE(IOCompletion);

	public:
		explicit IOCompletion(io_uring* ring, io_uring_cqe* cqe) noexcept;
		explicit IOCompletion(IOCompletion&& io_completion) noexcept;
		explicit IOCompletion(int result) noexcept;

		~IOCompletion();


	public:
		void handle_completion();


	public:
		int get_result() const;
		IOOperation* get_completed_operation() const { return completed_operation_.get(); }
		

	private:
		io_uring*				ring_ = nullptr;
		io_uring_cqe*			cqe_ = nullptr;
		int 					result_ = 0;
		ObjectPtr<IOOperation> 	completed_operation_ = nullptr;
	};
}