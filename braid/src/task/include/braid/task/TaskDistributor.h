#pragma once
#include <boost/lockfree/queue.hpp>

namespace braid {
    
    // 각 WorkerThread에 TaskSerializer을 분배하는 클래스
    class TaskSerializer;
    class TaskDistributor final {
    private:
        // 한번 처리시 최대 Task 수
        static constexpr int MAX_PROCESS_TASK_COUNT = 30;
        static constexpr int MAX_SERIALIZER_COUNT = 1024;

    public:
        TaskDistributor();
        ~TaskDistributor();
        
        void add_task_serializer(TaskSerializer* taskSerializer);
        void process_task_serializer();
        void process_task_serializer(TaskSerializer* taskSerializer);

 
    private:
        boost::lockfree::queue<TaskSerializer*> serializer_queue_;
    };

    static TaskDistributor g_task_distributor;

    thread_local static TaskSerializer* tl_process_task_serializer = nullptr;
}