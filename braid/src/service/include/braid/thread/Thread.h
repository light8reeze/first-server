#pragma once
#include <thread>
#include <util/Macro.h>

namespace braid {

	class Thread{
		NON_COPYABLE(Thread);

		
	public:
		Thread() = default;
		virtual ~Thread() = default;


	public:
		void start();
		void wait();
		void stop();
		void run();


		void set_thread_id(int id) { thread_id_ = id; }
		int get_thread_id() const { return thread_id_; }


	public:
		virtual void initialize();


	private:
		virtual int routine() = 0;


	private:
		int 		thread_id_ = -1;
		bool		is_stop_ = false;
		std::thread thread_;
		int 		tick_interval_ = 0;
	};
}