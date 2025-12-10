#include <task/TaskSerializer.h>
#include <task/TaskDistributor.h>
#include <task/Task.h>

namespace first {
    TaskSerializer::~TaskSerializer() {
        clear();
    }

    void TaskSerializer::push(ITask* task) {
        bool isEmpty = task_queue_.empty();
        task_queue_.push(task);

        if(isEmpty) {

            // 현재 처리중인 TaskSerializer가 없으면 바로 처리한다.
            if(nullptr == tl_process_task_serializer)
                g_task_distributor->process_task_serializer(this);
            else
            g_task_distributor->add_task_serializer(this);
        }
    }

    std::shared_ptr<ITask> TaskSerializer::pop() {
        ITask* task;
        task_queue_.pop(task);
        return std::shared_ptr<ITask>(task);
    }

    void TaskSerializer::set_destroy() {
        request_task(this, &TaskSerializer::clear);
        
        state_.store(DESTROYING);
    }

    TaskSerializer::STATE TaskSerializer::state() const {
        return state_.load();
    }

    void TaskSerializer::clear() {
        while (!task_queue_.empty()) {
            ITask* task;
            task_queue_.pop(task);
            delete task;
        }

        state_.store(DESTROYED);
    }

    bool TaskSerializer::empty() const {
        return task_queue_.empty();
    }
}