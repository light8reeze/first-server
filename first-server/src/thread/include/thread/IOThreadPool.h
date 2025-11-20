#pragma once
#include "WorkerThread.h"

namespace first {
    
    class IOOperation;
    class IOThreadPool {
    public:
        IOThreadPool(int num_threads = 4, int queue_depth = 1024);
        virtual ~IOThreadPool();


    public:
        void initialize();
        void start();
        void stop();
        void stop(int thread_index);
        void wait_all();

        void request_io(IOOperation* operation);
        void request_io(int thread_index, IOOperation* operation);


    private:
        int                                         num_threads_ = 4;
        int                                         queue_depth_ = 1024;
        std::vector<std::unique_ptr<WorkerThread>>  worker_threads_;


    private:
        static IOThreadPool* instance_;


    public:
        static IOThreadPool* get_instance() { return instance_; }
    };

}