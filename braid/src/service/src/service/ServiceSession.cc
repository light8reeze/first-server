#include <braid/service/ServiceSession.h>
#include <braid/service/Service.h>
#include <braid/service/Actor.h>

#include <braid/message/MessageHeader.h>
#include <braid/message/MessageDispatcher.h>

#include <braid/net/IOOperationRecv.h>
#include <braid/net/IOOperationAccept.h>
#include <braid/net/IOOperationClose.h>
#include <braid/net/IOOperationSendBuffer.h>

#include <iostream>

namespace braid {
	ServiceSession::ServiceSession(std::shared_ptr<Service>& service_instance) 
		: service_instance_(service_instance), main_actor_(nullptr) {
		assert(nullptr != service_instance);
	}

	void ServiceSession::request_receive() {
		std::span<char> remain_span = get_remain_span();
		IOOperationRecv* io_recv = new IOOperationRecv(shared_from_this(), remain_span);

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

	void ServiceSession::request_close() {
		IOOperationClose* io_close = new IOOperationClose(shared_from_this());

		if (auto service_ptr_ = service_instance_.lock())
			service_ptr_->request_io(io_close);
	}

	void ServiceSession::on_accepted() {
		if(main_actor_ == nullptr) {
			main_actor_ = new Actor();

			main_actor_->set_session(std::static_pointer_cast<ServiceSession>(shared_from_this()));
		}

		request_receive();
	}

	void ServiceSession::on_received(int bytes_received) {
		while (true) {
			if (commited_size_ < static_cast<int>(sizeof(MessageHeader)))
				break;

			std::span<char> received_span = get_received_span();
			MessageHeader* header = parse_message<MessageHeader>(received_span);
			if (nullptr == header || header->size < static_cast<int>(sizeof(MessageHeader)))
				break;

			if (received_span.size() < static_cast<size_t>(header->size))
				break;

			std::span<char> message_span = received_span.first(header->size);
			g_dispatcher.dispatch(header->message_type, message_span, main_actor_);
			process_completed(header->size);
		}

		request_receive();
	}

	void ServiceSession::on_closed() {
		if(nullptr != main_actor_) {
			main_actor_->on_closed();
			main_actor_.reset();
		}

		reset();
		if(auto service_ptr_ = service_instance_.lock())
			service_ptr_->on_session_closed(std::static_pointer_cast<ServiceSession>(shared_from_this()));
	}

	void ServiceSession::on_receive_failed(int error_code) {
		if(nullptr != main_actor_)
			main_actor_->request_close();
	}

	void ServiceSession::on_send_failed(int error_code) {
		if(nullptr != main_actor_)
			main_actor_->request_close();
	}
}
