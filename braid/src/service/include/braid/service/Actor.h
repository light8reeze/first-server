#pragma once
#include <braid/task/TaskSerializer.h>
#include <braid/message/SendBuffer.h>

namespace braid {

    // 서비스 로직에서 사용되는 오브젝트
    class ServiceSession;
    class Actor : public TaskSerializer {
    public:
        Actor() = default;
        virtual ~Actor() = default;


    public:
        void set_session(std::shared_ptr<ServiceSession> session) { session_ = session; }
        void reset_session() { session_.reset(); }
        void request_send(ObjectPtr<SendBuffer> send_buffer);


    private:
        std::shared_ptr<ServiceSession> session_;
    };
}