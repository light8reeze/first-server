#pragma once
#include <util/FirstServerPCH.h>
#include <netinet/in.h>
#include <memory>

namespace first {
	
	// io_uring를 사용하는 기본 오브젝트
	class IOUringObject : public std::enable_shared_from_this<IOUringObject> {
		NON_COPYABLE(IOUringObject);

	public:
		IOUringObject() = default;
		virtual ~IOUringObject() = default;


	public:
		virtual void on_received(int bytes_received) {};
		virtual void on_sent(int bytes_sent) {};
		virtual void on_accepted() {};


	public:
		virtual void on_accept_failed(int error_code) {};
		virtual void on_receive_failed(int error_code) {};
		virtual void on_send_failed(int error_code) {};

		
	public:
		void set_socket_fd(int fd) { socket_fd_ = fd; }
		void set_address(const struct sockaddr_in& addr) { address_ = addr; }

		char*				get_buffer() { return buffer_; }
		int					get_buffer_size() const { return 4096; }
		int					get_socket_fd() const { return socket_fd_; }
		struct sockaddr_in& get_address() { return address_; }


	protected:
		// TODO: 버퍼 클래스 추후 분리
		char				buffer_[4096] = { 0 };


	private:
		int					socket_fd_ = -1;
		struct sockaddr_in	address_ = { 0 };
	};
}