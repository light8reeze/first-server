#pragma once

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
}