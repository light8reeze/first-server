#pragma once
#include <braid/util/Macro.h>
#include <braid/memory/MemoryChunk.h>
#include <braid/memory/ThreadLocalCache.h>
#include <braid/memory/PooledObject.h>
#include <boost/lockfree/queue.hpp>
#include <atomic>
#include <new>
#include <type_traits>

namespace braid {

    template<typename T>
    class ObjectPool {
    public:
        static constexpr size_t DEFAULT_INITIAL_COUNT = 128;
        static constexpr size_t DEFAULT_CHUNK_SIZE = 128;
        static constexpr size_t DEFAULT_TLS_CACHE_SIZE = 32;
        static constexpr size_t DEFAULT_BATCH_SIZE = 16;

        struct Config {
            size_t initial_count = DEFAULT_INITIAL_COUNT;
            size_t chunk_size = DEFAULT_CHUNK_SIZE;
            size_t tls_cache_size = DEFAULT_TLS_CACHE_SIZE;
            size_t batch_size = DEFAULT_BATCH_SIZE;
        };

        explicit ObjectPool(const Config& config = Config{})
            : chunk_size_(config.chunk_size)
            , tls_cache_size_(config.tls_cache_size)
            , batch_size_(config.batch_size)
            , free_queue_(config.initial_count + 128)
            , chunk_head_(nullptr)
            , total_allocated_(0) {

            allocate_chunk(config.initial_count);
        }

        ~ObjectPool() {
            MemoryChunk<T>* chunk = chunk_head_.load();
            while (chunk != nullptr) {
                MemoryChunk<T>* next = chunk->next();
                delete chunk;
                chunk = next;
            }
        }

        NON_COPYABLE(ObjectPool)

    public:
        template<typename... Args>
        T* acquire(Args&&... args) noexcept {
            T* obj = get_tls_cache().try_acquire();

            if (obj == nullptr) {
                if (!free_queue_.pop(obj)) {
                    obj = batch_steal_from_central();

                    if (obj == nullptr) {
                        if (allocate_chunk(chunk_size_)) {
                            free_queue_.pop(obj);
                        } else {
                            // 할당중인경우 20회정도 재시도 한다.
                            for (int retry = 0; retry < 20; ++retry) {
                                if (free_queue_.pop(obj))
                                    break;
                            }
                        }
                    }
                }
            }

            if (obj != nullptr) {
                try {
                    new (obj) T(std::forward<Args>(args)...);
                } catch (...) {
                    free_queue_.push(obj);
                    return nullptr;
                }
            }

            return obj;
        }

        void release(T* obj) noexcept {
            if (obj == nullptr)
                return;

            obj->~T();

            if (!get_tls_cache().try_release(obj))
                free_queue_.push(obj);
            
            if (get_tls_cache().should_batch_return())
                batch_return_to_central();
        }

        template<typename... Args, typename U = T>
        requires std::is_base_of_v<PooledObject, U>
        ObjectPtr<U> acquire_ptr(Args&&... args) noexcept {
            U* obj = acquire(std::forward<Args>(args)...);
            if (obj == nullptr)
                return ObjectPtr<U>(nullptr);

            return ObjectPtr<U>(obj);
        }

        size_t total_allocated() const noexcept {
            return total_allocated_.load(std::memory_order_relaxed);
        }

    private:
        ThreadLocalCache<T>& get_tls_cache() noexcept {
            thread_local static ThreadLocalCache<T>* tls_cache = nullptr;

            if (tls_cache == nullptr)
                tls_cache = new ThreadLocalCache<T>(tls_cache_size_, batch_size_);

            return *tls_cache;
        }

        bool allocate_chunk(size_t count) noexcept {
            if (allocating_.test_and_set(std::memory_order_acquire))
                return false;

            MemoryChunk<T>* chunk = nullptr;

            try {
                chunk = new MemoryChunk<T>(count);
            } catch (...) {
                allocating_.clear(std::memory_order_release);
                return false;
            }

            for (size_t i = 0; i < count; ++i) {
                T* slot = chunk->get_slot(i);
                if (slot != nullptr)
                    free_queue_.push(slot);
            }

            MemoryChunk<T>* old_head = chunk_head_.load();
            do {
                chunk->set_next(old_head);
            } while (!chunk_head_.compare_exchange_weak(old_head, chunk));

            total_allocated_.fetch_add(count, std::memory_order_relaxed);

            allocating_.clear(std::memory_order_release);
            return true;
        }

        T* batch_steal_from_central() noexcept {
            T* batch[DEFAULT_BATCH_SIZE];
            size_t count = 0;
            for (size_t i = 0; i < batch_size_; ++i) {
                if (free_queue_.pop(batch[count]))
                    ++count;
                else
                    break;
            }

            if (count == 0)
                return nullptr;

            T* result = batch[0];
            if (count > 1)
                get_tls_cache().add_batch(batch + 1, count - 1);

            return result;
        }

        void batch_return_to_central() noexcept {
            T* batch[DEFAULT_BATCH_SIZE];
            size_t count = get_tls_cache().extract_batch(batch, DEFAULT_BATCH_SIZE);

            for (size_t i = 0; i < count; ++i) {
                free_queue_.push(batch[i]);
            }
        }

    private:
        size_t chunk_size_;
        size_t tls_cache_size_;
        size_t batch_size_;

        boost::lockfree::queue<T*> free_queue_;
        std::atomic<MemoryChunk<T>*> chunk_head_;
        std::atomic<size_t> total_allocated_;
        std::atomic_flag allocating_ = ATOMIC_FLAG_INIT;
    };
}
