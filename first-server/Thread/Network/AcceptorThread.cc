#include "AcceptorThread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

namespace first {

    AcceptorThread::AcceptorThread(int queue_depth /* = 1024 */, int port /* = 8080 */)
        : IORingThread(queue_depth) , port_(port){
    }

    AcceptorThread::~AcceptorThread() {
        io_uring_queue_exit(&ring_);
        close(listen_fd_);
    }

    void AcceptorThread::initialize() {
        IORingThread::initialize();

        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	    int enable = 1;
	    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

        struct sockaddr_in addr = { 0 };
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(listen_fd_, (struct sockaddr*)&addr, sizeof(addr));
        listen(listen_fd_, 128);

        // TODO : tptu

    	io_uring_submit(&ring_);
    }

    int AcceptorThread::routine() {
	    io_uring_cqe* cqe = nullptr;

        int ret = io_uring_wait_cqe(&ring_, &cqe);
        if (ret < 0) {
            std::cout << "io_uring_wait_cqe" << std::endl;
            return ret;
        }
        
        // TODO : Handle accepted connection
        on_accepted();

        io_uring_cqe_seen(&ring_, cqe);

        return 0;
    }

    void AcceptorThread::on_accepted() {
    }
}