#pragma once
#include "IOURingThread.h"

namespace braid {

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