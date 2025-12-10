#pragma once
#include <boost/lockfree/queue.hpp>
#include <task/Task.h>

namespace first {

    class ITask;
    class TaskSerializer {
    public:
        enum STATE {
            RUNNING,
            DESTROYING,
            DESTROYED
        };

    public:
        TaskSerializer() = default;
        virtual ~TaskSerializer();


    private:
        void push(ITask* task);


    public:
        template<typename Obj, typename Func, typename... Args>
        void request_task(Obj&& obj, Func&& func, Args&&... args) {
            if (state_.load() != RUNNING)
                return;

            push(new Task<Obj, Func, Args...>(std::forward<Obj>(obj), std::forward<Func>(func), std::forward<Args>(args)...));
        }

        std::shared_ptr<ITask> pop();

        void set_destroy();

        STATE state() const;

        void clear();

        bool empty() const;


    private:
        boost::lockfree::queue<ITask*>  task_queue_;
        std::atomic<STATE>              state_{RUNNING};
    };
}