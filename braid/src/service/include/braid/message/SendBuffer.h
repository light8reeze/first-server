#pragma once
#include <span>
#include <cstring>
#include <string>
#include <type_traits>

#include <braid/task/RefCountable.h>

namespace braid {

    class SendBuffer : public RefCountable {
    private:
        static constexpr int MAX_MESSAGE_SIZE = 2048;

    public:
        SendBuffer() = default;
        ~SendBuffer() = default;


    public:
        template<typename... Args>
        void write(const Args&... args) {
            static_assert((std::is_trivially_copyable_v<Args> && ...));

            constexpr int total_size = (sizeof(Args) + ... + 0);
            if (MAX_MESSAGE_SIZE < size_ + total_size)
                return;

            char* out = buffer_ + size_;
            auto append_one = [&out](const auto& x) {
                std::memcpy(out, &x, sizeof(x));
                out += sizeof(x);
            };

            (append_one(args), ...);
            size_ += total_size;
        }

        void write(const std::string& str) {
            if (MAX_MESSAGE_SIZE < size_ + str.size())
                return;

            std::memcpy(buffer_ + size_, str.data(), str.size());
            size_ += str.size();
        }

        void write(const char* str, int size) {
            if (MAX_MESSAGE_SIZE < size_ + size)
                return;

            std::memcpy(buffer_ + size_, str, size);
            size_ += size;
        }

        std::span<char> get_buffer() { return std::span<char>(buffer_, size_); }
        void reset() { size_ = 0; }

    private:
        char buffer_[MAX_MESSAGE_SIZE];
        int size_ = 0;
    };
}
