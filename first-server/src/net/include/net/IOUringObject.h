#pragma once
#include <util/FirstServerPCH.h>
#include <netinet/in.h>

namespace first {
	
	// io_uring �̺�Ʈ ��ü �⺻ Ŭ����
	class IOUringObject
	{
		NON_COPYABLE(IOUringObject);

	public:
		IOUringObject() = default;
		virtual ~IOUringObject() = default;


	public:
		virtual void on_received(int bytes_received) {};
		virtual void on_sent(int bytes_sent) {};
		virtual void on_accepted() {};


	public:
		void set_socket_fd(socket_fd fd) { socket_fd_ = fd; }
		void set_address(const struct sockaddr_in& addr) { address_ = addr; }

		char*				get_buffer() { return buffer_; }
		int					get_buffer_size() const { return 4096; }
		socket_fd			get_socket_fd() const { return socket_fd_; }
		struct sockaddr_in& get_address() { return address_; }


	private:
		// TODO: ���� ���� �и�
		char				buffer_[4096] = { 0 };

		socket_fd			socket_fd_ = -1;
		struct sockaddr_in	address_ = { 0 };
	};
}