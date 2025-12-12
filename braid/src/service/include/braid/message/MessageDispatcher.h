#pragma once
#include <span>
#include <functional>
#include <unordered_map>
#include <braid/service/Actor.h>

namespace braid {

    class Actor;

    using MessageHandler = std::function<void(std::span<char>& data, ObjectPtr<Actor>)>;
    class MessageDispatcher {
    public:
        MessageDispatcher() = default;
        ~MessageDispatcher() = default;


    public:
        bool dispatch(int message_type, std::span<char>& data, ObjectPtr<Actor> actor);
        void register_handler(int message_type, MessageHandler&& handler);


    private:
        std::unordered_map<int, MessageHandler> handlers_;
    };

    static MessageDispatcher g_dispatcher;
}
