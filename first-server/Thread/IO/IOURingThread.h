#pragma once
#include "Thread.h"
#include <IOUring.h>

namespace first {

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