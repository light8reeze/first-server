#pragma once
#include <braid/util/Macro.h>
#include <vector>
#include <cstddef>
#include <algorithm>

namespace braid {

    template<typename T>
    class ThreadLocalCache {
    public:
        explicit ThreadLocalCache(size_t max_cache_size, size_t batch_size)
            : max_cache_size_(max_cache_size)
            , batch_size_(batch_size) {
            cache_.reserve(max_cache_size);
        }

        NON_COPYABLE(ThreadLocalCache)

        T* try_acquire() noexcept {
            if (cache_.empty()) {
                return nullptr;
            }
            T* obj = cache_.back();
            cache_.pop_back();
            return obj;
        }

        bool try_release(T* obj) noexcept {
            if (cache_.size() < max_cache_size_) {
                cache_.push_back(obj);
                return true;
            }
            return false;
        }

        bool should_batch_return() const noexcept {
            return cache_.size() > (max_cache_size_ * 3 / 4);
        }

        size_t extract_batch(T** out_buffer, size_t buffer_size) noexcept {
            size_t count = std::min({batch_size_, buffer_size, cache_.size()});
            for (size_t i = 0; i < count; ++i) {
                out_buffer[i] = cache_.back();
                cache_.pop_back();
            }
            return count;
        }

        void add_batch(T** objects, size_t count) noexcept {
            for (size_t i = 0; i < count && cache_.size() < max_cache_size_; ++i) {
                cache_.push_back(objects[i]);
            }
        }

        size_t size() const noexcept { return cache_.size(); }

    private:
        template<typename U> friend class ObjectPool;
        size_t max_cache_size_;
        size_t batch_size_;
        std::vector<T*> cache_;
    };

}
