#pragma once
#include <braid/service/ServiceBuilder.h>
#include <braid/service/IOPool.h>

#include <vector>
#include <atomic>
#include <mutex>

namespace braid {
    class IOOperation;
    class ServiceSession;
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

        template<typename T, typename... Args>
        void request_io(Args&&... args) {
            T* operation = io_pool_.acquire<T>(std::forward<Args>(args)...);
            if (operation == nullptr)
                return;

            request_io(operation);
        }

		void request_io(IOOperation* operation);
		void request_io(int thread_index, IOOperation* operation);

        void on_session_closed(std::shared_ptr<ServiceSession> session);

        void request_accept_one();


    private:
        virtual void initialize_threads();


	private:
		std::vector<std::unique_ptr<WorkerThread>>  worker_threads_{};

		std::shared_ptr<ServiceSession> acceptor_object_ = nullptr;

        std::atomic<int> request_index_ = 0;
    
        // Session Pool : 세션 개수는 고정하기 때문에 ObjectPool을 굳이 사용하지 않는다.
		std::vector<std::shared_ptr<ServiceSession>>  sessions_{};
        boost::lockfree::queue<ServiceSession*> sessions_queue_{static_cast<size_t>(session_count_)};
        IOPool io_pool_;


    protected:
		int session_count_          = 1000;
		int thread_count_           = 4;
		int queue_depth_per_thread_ = 1024;
        int backlog_                = 4096;
    };
}