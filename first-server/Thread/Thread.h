#pragma once

#include <thread>
#include "FirstServerPCH.h"

namespace first {

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