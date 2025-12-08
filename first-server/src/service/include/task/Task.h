#pragma once
#include <tuple>
#include <memory>
#include <functional>

namespace first {

    class ITask {
    public:
        virtual void invoke() = 0;
        virtual ~ITask(){};
    };

    template <typename Obj, typename Func, typename... Args>
    class Task : public ITask {
    public:
        template<typename ObjPtr>
        Task(ObjPtr obj, Func func, Args... args) : args_(func, std::move(obj), std::forward<Args>(args)...) {}
        virtual ~Task() = default;

    public:
        virtual void invoke() override {
            std::apply(std::invoke, args_);
        }

    private:
        std::tuple<Func, std::shared_ptr<Obj>, Args...> args_;
    };
}