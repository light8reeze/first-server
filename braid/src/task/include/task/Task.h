#pragma once
#include <tuple>
#include <functional>
#include <task/RefCountable.h>
#include <util/Utility.h>

namespace first {

    class ITask : public RefCountable {
    public:
        virtual void invoke() = 0;
        virtual ~ITask(){}
    };

    // TaskSerializer에 요청할 작업에 대한 클래스
    // TaskSerializer에 의해서만 생성되어야 함
    template <typename Obj, typename Func, typename... Args>
    requires std::is_base_of_v<RefCountable, Obj>
    class Task : public ITask {
    public:
        explicit Task(Obj* obj, Func&& func, Args... args) : args_(std::move(func), obj, std::forward<Args>(args)...) {

            // RefCountable 인경우 ref_count를 증가시킨다.
            for_each_tuple(args_, [](auto& arg) {
                if constexpr (std::is_base_of_v<RefCountable, std::decay_t<decltype(arg)>>)
                    arg->add_ref();
            });

        }

        template<typename ObjPtr>
        requires std::is_base_of_v<RefCountable, std::decay_t<ObjPtr>::T>
        explicit Task(ObjPtr&& obj, Func&& func, Args... args) : args_(std::move(func), obj.get(), std::forward<Args>(args)...) {
        
            for_each_tuple(args_, [](auto& arg) {
                if constexpr (std::is_base_of_v<RefCountable, std::decay_t<decltype(arg)>>)
                    arg->add_ref();
            });
        }

        virtual ~Task() override {

            for_each_tuple(args_, [](auto& arg) {
                if constexpr (std::is_base_of_v<RefCountable, std::decay_t<decltype(arg)>>)
                    arg->release();
            });
        }

    public:
        virtual void invoke() override {
            std::apply([](auto&&... args) {
                return std::invoke(std::forward<decltype(args)>(args)...);
            }, args_);
        }

    private:
        std::tuple<Func, Obj*, Args...> args_;
    };
}