#pragma once
#include <boost/intrusive_ptr.hpp>
#include <assert.h>

namespace first {

    // boost::instrusive_ptr을 사용하기 위한 클래스
    // lock-free를 사용하기 위해 자체 ref_count를 사용한다.
    class RefCountable {
    public:
        RefCountable() = default;
        virtual ~RefCountable() = default;


    public:
        friend void intrusive_ptr_add_ref(RefCountable* ptr) {
            if(nullptr != ptr)
                ptr->ref_count_.fetch_add(1, std::memory_order_relaxed); 
        }
        
        friend void intrusive_ptr_release(RefCountable* ptr) { 
            assert(nullptr != ptr);
            assert(0 < ptr->ref_count_);

            if(nullptr != ptr && ptr->ref_count_.fetch_sub(1, std::memory_order_acq_rel) == 1)
                delete ptr;
        }

        void add_ref() {
            intrusive_ptr_add_ref(this);
        }

        void release() {
            intrusive_ptr_release(this);
        }

    private:
        std::atomic<int> ref_count_ = 0;
    };

    template <typename T>
    requires std::is_base_of_v<RefCountable, T>
    using ObjectPtr = boost::intrusive_ptr<T>;
}