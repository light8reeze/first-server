#pragma once

#include "IORingThread.h"
#include "FirstServerPCH.h"

namespace first {

    class AcceptorThread : public IORingThread {
        public:
            AcceptorThread(int queue_depth = 1024, int port = 8080);
            virtual ~AcceptorThread();


        public:
            virtual void initialize() override;


        private:
            virtual int routine() override;
            virtual void on_accepted();


        protected:
            SocketFd listen_fd_ = -1;
            int port_ = 8080;
    };
}