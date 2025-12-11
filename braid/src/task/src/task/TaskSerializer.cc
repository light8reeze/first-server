#include <braid/task/TaskSerializer.h>
#include <braid/task/TaskDistributor.h>
#include <braid/task/Task.h>

namespace braid {
    TaskSerializer::TaskSerializer() : task_queue_(MAX_TASK_COUNT) {
    }

    TaskSerializer::~TaskSerializer() {
        clear();
    }

    void TaskSerializer::push(ITask* task) {
        bool wasEmpty = task_queue_.empty();
        task_queue_.push(task);

        if(true == wasEmpty) {

            // 현재 처리중인 TaskSerializer가 없으면 바로 처리한다.
            if(nullptr == tl_process_task_serializer)
                g_task_distributor.process_task_serializer(this);
            else
                g_task_distributor.add_task_serializer(this);
        }
    }

    ObjectPtr<ITask> TaskSerializer::pop() {
        ITask* task;
        task_queue_.pop(task);
        return ObjectPtr<ITask>(task);
    }

    void TaskSerializer::set_destroy() {
        request_task(this, &TaskSerializer::clear);
        
        state_.store(DESTROYING);
    }

    bool TaskSerializer::pop(ObjectPtr<ITask>& task) {
        ITask* taskPtr;
        
        if(true == task_queue_.pop(taskPtr)) {
            task = ObjectPtr<ITask>(taskPtr);
            return true;
        }
        
        return false;
    }

    TaskSerializer::STATE TaskSerializer::state() const {
        return state_.load();
    }

    void TaskSerializer::clear() {
        while (false == task_queue_.empty()) {
            ITask* task;
            task_queue_.pop(task);

            if(nullptr != task)
                task->release();
        }

        state_.store(DESTROYED);
    }

    bool TaskSerializer::empty() const {
        return task_queue_.empty();
    }
}