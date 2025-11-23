#pragma once
#include <net/IOOperation.h>

struct io_uring;

namespace first {
    // io_uring 이벤트를 발생시키기 위해 사용하는 오퍼레이션
    // 게임 서버 로직에서 이벤트 발생 필요시 해당 클래스를 상속받아서 추가한다. 
    class IOOperationNop : public IOOperation {
    public:
        IOOperationNop(IOUringObject* io_object);
        virtual ~IOOperationNop() = default;

        
    public:
        void request_io(io_uring* ring) override;
        void handle_io_completion(int result) override;


    protected:
        virtual void on_event_request() = 0;
        virtual void on_event_completed() = 0;
    };
}