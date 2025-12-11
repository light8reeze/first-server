#pragma once
#include <liburing.h>
#include <boost/lockfree/queue.hpp>
#include <net/IOCompletion.h>

namespace braid {

	class IOUringObject;
	class IOOperation;
	class IORequestQueue
	{
	public:
		IORequestQueue(int queue_depth = 1024);
		~IORequestQueue();


	public:
		IOCompletion	wait_one(int timeout_ms = -1);
		void			submit();

		void			push_request(IOOperation* operation);
		void			flush_requests();


	public:
		io_uring* get_ring() { return &ring_; }


	private:
		io_uring ring_;
		
		boost::lockfree::queue<IOOperation*> request_queue_;
	};

}