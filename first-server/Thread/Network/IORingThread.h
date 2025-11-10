#include "Thread.h"
#include <liburing.h>

namespace first {

    class IORingThread : public Thread {

    public:
        IORingThread(int queue_depth = 1024);
        virtual ~IORingThread();


    public:
        io_uring* getRing();


    protected:
        io_uring ring_;
    };
}