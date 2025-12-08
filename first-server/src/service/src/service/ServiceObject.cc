#include <service/ServiceObject.h>
#include <service/Service.h>

#include <net/IOOperationRecv.h>
#include <net/IOOperationSend.h>
#include <net/IOOperationAccept.h>

#include <iostream>

namespace first {
	ServiceObject::ServiceObject(std::shared_ptr<Service>& service_instance) 
		: service_instance_(service_instance) {
		
		assert(nullptr != service_instance);
	}

	void ServiceObject::request_receive() {
		IOOperationRecv* io_recv = new IOOperationRecv(shared_from_this());

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_recv);
	}

	void ServiceObject::request_send() {
		IOOperationSend* io_send = new IOOperationSend(shared_from_this());

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_send);
	}

	void ServiceObject::request_accept(int accept_fd_) {
		IOOperationAccept* io_accept = new IOOperationAccept(shared_from_this(), accept_fd_);

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_accept);
	}

	void ServiceObject::on_accepted() {
		std::cout << "on accepted" << std::endl;
		request_receive();
	}

	void ServiceObject::on_received(int bytes_received) {
		std::cout << "bytes received : " << bytes_received << std::endl;
		
		// TODO: 이벤트 처리

		request_receive();
	}
}