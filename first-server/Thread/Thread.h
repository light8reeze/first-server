#include <thread>

namespace first {

	class Thread{

	public:
		Thread(const Thread&) = delete;

	public:
		void initialize();

		void start();

		void wait();

		void stop();

	public:
		virtual int run() = 0;

	private:
		bool		isStop_ = false;
		std::thread thread_;
	};

}