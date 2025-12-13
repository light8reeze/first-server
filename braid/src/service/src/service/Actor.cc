#include <braid/service/Actor.h>
#include <braid/service/ServiceSession.h>

namespace braid {
    void Actor::request_send(ObjectPtr<SendBuffer> send_buffer) {
        if (session_)
            session_->request_send(send_buffer);
    }
}