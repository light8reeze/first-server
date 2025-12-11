#include <thread/WorkerThread.h>
#include <task/TaskDistributor.h>
#include <iostream>

namespace first {

    WorkerThread::WorkerThread(int queue_depth /* = 1024 */) 
        : IOURingThread(queue_depth) {
    }

    WorkerThread::~WorkerThread() {
    }

    void WorkerThread::initialize() {
        IOURingThread::initialize();
        
    }

    int WorkerThread::routine() {
		IOCompletion completion = ring_queue_.wait_one(0);

        completion.handle_completion();
 
        ring_queue_.flush_requests();

        g_task_distributor.process_task_serializer();

        return 0;
    }
}