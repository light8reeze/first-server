#pragma once
#include <braid/util/Macro.h>
#include <cstddef>
#include <new>

namespace braid {

template<typename T>
class MemoryChunk {
    NON_COPYABLE(MemoryChunk)
    
public:
    explicit MemoryChunk(size_t count)
        : count_(count)
        , memory_(static_cast<std::byte*>(
            ::operator new(sizeof(T) * count, std::align_val_t{alignof(T)})))
        , next_(nullptr) {
    }

    ~MemoryChunk() {
        ::operator delete(memory_, std::align_val_t{alignof(T)});
    }


public:
    T* get_slot(size_t index) noexcept {
        if (index >= count_) return nullptr;
        return reinterpret_cast<T*>(memory_ + sizeof(T) * index);
    }

    size_t count() const noexcept { return count_; }

    MemoryChunk* next() const noexcept { return next_; }
    void set_next(MemoryChunk* next) noexcept { next_ = next; }

private:
    template<typename U> friend class ObjectPool;
    size_t count_;
    std::byte* memory_;
    MemoryChunk* next_;
};

}
