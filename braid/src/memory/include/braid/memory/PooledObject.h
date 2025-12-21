#pragma once
#include <braid/task/RefCountable.h>

namespace braid {
    
    class PooledObject : public RefCountable {
    public:
        PooledObject() = default;
        virtual ~PooledObject() = default;

    protected:
        virtual void on_zero_ref() = 0;

        void on_final_release() override {
            on_zero_ref();
        }
    };

}
