#include <braid/service/ServiceSession.h>
#include <braid/service/Service.h>

#include <braid/net/IOOperationRecv.h>
#include <braid/net/IOOperationSend.h>
#include <braid/net/IOOperationAccept.h>

#include <iostream>

namespace braid {
	ServiceSession::ServiceSession(std::shared_ptr<Service>& service_instance) 
		: service_instance_(service_instance) {
		
		assert(nullptr != service_instance);
	}

	void ServiceSession::request_receive() {
		IOOperationRecv* io_recv = new IOOperationRecv(shared_from_this());

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_recv);
	}

	void ServiceSession::request_send() {
		IOOperationSend* io_send = new IOOperationSend(shared_from_this());

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_send);
	}

	void ServiceSession::request_accept(int accept_fd_) {
		IOOperationAccept* io_accept = new IOOperationAccept(shared_from_this(), accept_fd_);

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_accept);
	}

	void ServiceSession::on_accepted() {
		std::cout << "on accepted" << std::endl;
		request_receive();
	}

	void ServiceSession::on_received(int bytes_received) {
		std::cout << "bytes received : " << bytes_received << std::endl;
		

		request_receive();
	}
}