#pragma once
#include "FirstServerPCH.h"
#include "IORingThread.h"
#include <boost/lockfree/queue.hpp>

namespace first {

    class WorkerThread : public IOURingThread {

    public:
        WorkerThread(int queue_depth = 1024);
        virtual ~WorkerThread();


    public:
        virtual void initialize() override;

        
    private:
        virtual int routine() override;
    };
}