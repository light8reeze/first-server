#pragma once
#include "Thread.h"
#include <net/IORequestQueue.h>

namespace braid {

    class IOURingThread : public Thread {

    public:
        IOURingThread(int queue_depth = 1024);
        virtual ~IOURingThread();


    public:
        void request_io(IOOperation* operation);


    protected:
		IORequestQueue ring_queue_;
    };
}