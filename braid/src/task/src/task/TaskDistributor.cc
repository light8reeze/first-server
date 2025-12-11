#include <task/TaskDistributor.h>
#include <task/TaskSerializer.h>
#include <cassert>

namespace first {

    void TaskDistributor::add_task_serializer(TaskSerializer* task_serializer) {
        serializer_queue_.push(task_serializer);
    }

    // 각 WorkerThread에서 호출하여 TaskSerializer를 처리한다.
    void TaskDistributor::process_task_serializer() {
        assert(tl_process_task_serializer == nullptr);

        TaskSerializer* task_serializer = nullptr;
        serializer_queue_.pop(task_serializer);

        if(nullptr == task_serializer)
            return;
        
        process_task_serializer(task_serializer);
    }

    void TaskDistributor::process_task_serializer(TaskSerializer* task_serializer) {
        assert(tl_process_task_serializer == nullptr);

        tl_process_task_serializer = task_serializer;

        ObjectPtr<ITask> task;
        int process_count = 0;
        while (task = tl_process_task_serializer->pop()) {
            task->invoke();

            process_count++;
            if (process_count >= MAX_PROCESS_TASK_COUNT)
                break;
        }

        // TaskSerializer가 소멸되어야 하는 상태면 삭제해준다.
        if(tl_process_task_serializer->state() == TaskSerializer::DESTROYED)
            delete tl_process_task_serializer;
        // TaskSerializer가 비어있지 않으면 다시 큐에 넣는다.
        else if(!tl_process_task_serializer->empty())
            serializer_queue_.push(tl_process_task_serializer);

        tl_process_task_serializer = nullptr;
    }
}
