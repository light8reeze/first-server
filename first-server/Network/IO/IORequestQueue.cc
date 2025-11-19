#include "IORequestQueue.h"

namespace first {

	IORequestQueue::IORequestQueue(int queue_depth /*= 1024*/) 
		: request_queue_(queue_depth) {
		::io_uring_queue_init(queue_depth, &ring_, 0);
	}

	IORequestQueue::~IORequestQueue() {
		::io_uring_queue_exit(&ring_);
	}

	void IORequestQueue::set_accept(IOUringObject* acceptor, IOUringObject* io_object) {
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
		
		socklen_t addrlen = sizeof(io_object->get_address());
		struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&io_object->get_address());

		::io_uring_prep_accept(sqe, acceptor->get_socket_fd(), addr, &addrlen, 0);
		::io_uring_sqe_set_data(sqe, io_object);
	}

	void IORequestQueue::set_receive(IOUringObject* io_object) {
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);

		::io_uring_prep_recv(sqe, io_object->get_socket_fd(), io_object->get_buffer(), io_object->get_buffer_size(), 0);
		::io_uring_sqe_set_data(sqe, io_object);
	}

	void IORequestQueue::set_send(IOUringObject* io_object) {
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);

		::io_uring_prep_send(sqe, io_object->get_socket_fd(), io_object->get_buffer(), io_object->get_buffer_size(), 0);
		::io_uring_sqe_set_data(sqe, io_object);
	}

	IOCompletion IORequestQueue::wait(int timeout_ms /*= -1*/) {
		struct __kernel_timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = timeout_ms * 1000000;

		io_uring_cqe* cqe;
		int ret = 0;
		if (0 <= timeout_ms)
			ret = ::io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
		else
			ret = ::io_uring_wait_cqe(&ring_, &cqe);

		if (ret < 0) {
			// TODO: ����ó�� �߰�
			perror("io_uring_wait_cqe");
			return IOCompletion(nullptr, nullptr);
		}

		return IOCompletion(&ring_, cqe);
	}

	void IORequestQueue::submit() {
		::io_uring_submit(&ring_);
	}

	void IORequestQueue::push_request(IOOperation* operation) {
		while (!request_queue_.push(operation)) {
			// TODO: push 회수 제한, 예외처리
		}
	}

	void IORequestQueue::flush_requests() {
		IOOperation* operation = nullptr;
		while (request_queue_.pop(operation)) {
			if (operation)
				operation->request_io(&ring_);
		}

		submit();
	}
}