#pragma once
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <task/Task.h>

namespace first {

    class ITask;
    class TaskSerializer {
    public:
        TaskSerializer() = default;
        virtual ~TaskSerializer() = default;


    private:
        void push(ITask* task);


    public:
        template<typename Obj, typename Func, typename... Args>
        void request_task(Obj&& obj, Func&& func, Args&&... args) {
            push(new Task<Obj, Func, Args...>(std::forward<Obj>(obj), std::forward<Func>(func), std::forward<Args>(args)...));
        }

        std::shared_ptr<ITask> pop();


    private:
        boost::lockfree::queue<ITask*> task_queue_;
    };
}