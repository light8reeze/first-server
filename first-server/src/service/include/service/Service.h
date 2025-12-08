#pragma once
#include <service/ServiceBuilder.h>
#include <vector>
#include <atomic>

namespace first {
    class IOOperation;
    class ServiceObject;
    class WorkerThread;
    class Service : public std::enable_shared_from_this<Service> {
	private:
		friend class ServiceBuilder<Service>;
		Service();


	public:
        virtual ~Service();


    public:
        virtual bool initialize();


    public:
        bool run();

		void request_io(IOOperation* operation);
		void request_io(int thread_index, IOOperation* operation);


    private:
        virtual void initialize_threads();


	protected:
		std::vector<std::unique_ptr<WorkerThread>>  worker_threads_{};
		std::vector<std::shared_ptr<ServiceObject>>  sessions_{};

		std::shared_ptr<ServiceObject> acceptor_object_ = nullptr;

		int session_count_          = 1000;
		int thread_count_           = 4;
		int queue_depth_per_thread_ = 1024;

        std::atomic<int> request_index_ = 0;
    };
}