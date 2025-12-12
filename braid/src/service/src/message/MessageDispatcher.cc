#include <braid/message/MessageDispatcher.h>

namespace braid {
    bool MessageDispatcher::dispatch(int message_type, std::span<char>& data, ObjectPtr<Actor> actor) {
        if (handlers_.find(message_type) == handlers_.end())
            return false;
        
        handlers_[message_type](data, actor);
        return true;
    }

    void MessageDispatcher::register_handler(int message_type, MessageHandler&& handler) {
        if (handlers_.find(message_type) != handlers_.end())
            return;
        
        handlers_[message_type] = handler;
    }
}
