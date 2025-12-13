#pragma once
#include <braid/service/Actor.h>
#include <braid/message/SendBuffer.h>

namespace braid {
    class MessageHeader {
    public:
        int     size;
        uint    message_type;
    };

    template<typename T>
    requires std::is_base_of_v<MessageHeader, T>
    static T* parse_message(std::span<char>& data) {
        return reinterpret_cast<T*>(data.data());
    }

    template<typename... Args>
    static void SendMessage(ObjectPtr<Actor> actor, Args&&... args) {
        if (nullptr == actor)
            return; 

        ObjectPtr<SendBuffer> send_buffer = new SendBuffer();
        send_buffer->write(std::forward<Args>(args)...);
        actor->request_send(send_buffer);
    }

    template<typename... Args>
    static void SendMessage(std::vector<ObjectPtr<Actor>>& actor_list, Args&&... args) {
        ObjectPtr<SendBuffer> send_buffer = new SendBuffer();
        send_buffer->write(std::forward<Args>(args)...);

        for (auto& actor : actor_list) {
            if (nullptr == actor)
                continue;

            actor->request_send(send_buffer);
        }
    }
}