#pragma once
#include <boost/lockfree/queue.hpp>
#include <braid/task/RefCountable.h>
#include <braid/task/Task.h>

namespace braid {

    class ITask;
    class TaskSerializer final : public RefCountable {
    private:
        static constexpr int MAX_TASK_COUNT = 1024;

        
    public:
        enum STATE {
            RUNNING,
            DESTROYING,
            DESTROYED
        };
        

    public:
        TaskSerializer();
        ~TaskSerializer();


    private:
        void push(ITask* task);

        
    public:
        template<typename Obj, typename Func, typename... Args>
        requires std::is_base_of_v<RefCountable, Obj>
        void request_task(Obj* obj, Func&& func, Args&&... args) {
            if (state_.load() != RUNNING)
                return;

            push(new Task<Obj, Func, Args...>(obj, std::forward<Func>(func), std::forward<Args>(args)...));
        }
        
        template<typename ObjPtrT, typename Func, typename... Args>
        requires std::is_base_of_v<RefCountable, std::decay_t<ObjPtrT>::T>
        void request_task(ObjPtrT&& obj, Func&& func, Args&&... args) {
            if (state_.load() != RUNNING)
                return;

            push(new Task<ObjPtrT&&, Func, Args...>(std::forward<ObjPtrT>(obj), std::forward<Func>(func), std::forward<Args>(args)...));
        }

        ObjectPtr<ITask> pop();

        bool pop(ObjectPtr<ITask>& task);

        void set_destroy();

        STATE state() const;

        void clear();

        bool empty() const;


    private:
        boost::lockfree::queue<ITask*>  task_queue_;
        std::atomic<STATE>              state_{RUNNING};
    };
}