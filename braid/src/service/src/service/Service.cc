#include <braid/service/Service.h>
#include <braid/service/ServiceSession.h>
#include <braid/thread/WorkerThread.h>
#include <braid/net/IOOperation.h>

namespace braid {
	Service::Service() 
		: worker_threads_() {
	}

	Service::~Service() {
		worker_threads_.clear();
	}

	bool Service::initialize() {
		initialize_threads();

		// TODO: Session Factory를 사용하여 생성
		sessions_.reserve(session_count_);
		std::shared_ptr<Service> self = shared_from_this();
		for (int i = 0; i < session_count_; ++i)
		{
			sessions_.emplace_back(std::make_shared<ServiceSession>(self));
			sessions_.back()->request_accept(acceptor_object_->get_socket_fd());
		}

		return true;
	}

	bool Service::run() {

		for (auto& thread : worker_threads_)
			thread->start();

		for (auto& thread : worker_threads_)
			thread->wait();

		return true;
	}

	void Service::request_io(IOOperation* operation) {
		int thread_index = request_index_++ % thread_count_;

		request_io(thread_index, operation);
	}

	void Service::request_io(int thread_index, IOOperation* operation) {
		if (thread_index < 0 || thread_index >= thread_count_)
			return;

		worker_threads_[thread_index]->request_io(operation);
	}

	void Service::initialize_threads() {
		for (int i = 0; i < thread_count_; ++i) {
			worker_threads_.emplace_back(std::make_unique<WorkerThread>(queue_depth_per_thread_));
			worker_threads_.back()->set_thread_id(i);
			worker_threads_.back()->initialize();
		}
	}
}