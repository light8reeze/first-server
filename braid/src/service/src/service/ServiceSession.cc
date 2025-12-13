#include <braid/service/ServiceSession.h>
#include <braid/service/Service.h>
#include <braid/service/Actor.h>

#include <braid/message/MessageHeader.h>
#include <braid/message/MessageDispatcher.h>

#include <braid/net/IOOperationRecv.h>
#include <braid/net/IOOperationAccept.h>
#include <braid/net/IOOperationSendBuffer.h>

#include <iostream>

namespace braid {
	ServiceSession::ServiceSession(std::shared_ptr<Service>& service_instance) 
		: service_instance_(service_instance), main_actor_(new Actor()) {
		
		assert(nullptr != service_instance);
	}

	void ServiceSession::request_receive() {
		std::span<char> received_span = get_received_span();
		IOOperationRecv* io_recv = new IOOperationRecv(shared_from_this(), received_span);

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_recv);
	}

	void ServiceSession::request_send(ObjectPtr<SendBuffer> send_buffer) {
		IOOperationSendBuffer* io_send = new IOOperationSendBuffer(shared_from_this(), send_buffer);

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

		do {
			if (bytes_received < sizeof(MessageHeader))
				break;
			
			std::span<char> message_span = get_received_span();
			MessageHeader* header = parse_message<MessageHeader>(message_span);
			if (message_span.size() < header->size)
				break;

			message_span = message_span.subspan(header->size);
			g_dispatcher.dispatch(header->message_type, message_span, main_actor_);
			process_completed(header->size);
		}
		while(false);

		request_receive();
	}
}