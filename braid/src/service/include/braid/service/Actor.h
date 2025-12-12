#pragma once
#include <braid/task/TaskSerializer.h>

namespace braid {

    // 서비스 로직에서 사용되는 오브젝트
    class Actor : public TaskSerializer {
    public:
        Actor() = default;
        virtual ~Actor() = default;
    };
}