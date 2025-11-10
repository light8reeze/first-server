#pragma once

#include "FirstServerPCH.h"
#include "IORingThread.h"

namespace first {

    class WorkerThread : public IORingThread {

    public:
        WorkerThread(int queue_depth = 1024);
        virtual ~WorkerThread();


    public:
        virtual void initialize() override;

        
    private:
        virtual int routine() override;
    };
}