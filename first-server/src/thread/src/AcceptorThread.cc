#include <thread/AcceptorThread.h>
#include <net/IOCompletion.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

namespace first {

    AcceptorThread::AcceptorThread(int queue_depth /* = 1024 */, int port /* = 8080 */)
        : IOURingThread(queue_depth) , port_(port){
    }

    AcceptorThread::~AcceptorThread() {
        close(acceptor_object_.get_socket_fd());
    }

    void AcceptorThread::initialize() {
        IOURingThread::initialize();

        socket_fd listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	    int enable = 1;
	    ::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

        struct sockaddr_in addr = { 0 };
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;

        ::bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
        ::listen(listen_fd, 128);

		acceptor_object_.set_socket_fd(listen_fd);
        acceptor_object_.set_address(addr);

        for (IOUringObject& client : client_object_) {
            ring_queue_.set_accept(&acceptor_object_, &client);
        }

        ring_queue_.submit();
    }

    int AcceptorThread::routine() {

		IOCompletion completion = ring_queue_.wait();

        if (0 < completion.get_result())
            completion.handle_completion();

        return 0;
    }

	void AcceptorThread::on_accepted(IOUringObject* client) {
		// TODO : Handle accepted connection
        if (nullptr == client)
            return;
    }
}