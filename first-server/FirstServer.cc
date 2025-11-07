#include "FirstServer.h"
#include <liburing.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

#define QUEUE_DEPTH 256
#define PORT 8080

enum {
	EVENT_ACCEPT,
	EVENT_READ,
	EVENT_WRITE
};

typedef struct {
	int type;
	int fd = 0;
	char buffer[1024];

} conn_info;

int setup_listening_socket(int port) {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	int enable = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(sock_fd, 128);

	return sock_fd;
}

conn_info conn_infos[10];

int worker_thread(int socket_fd, io_uring* ring) {
	struct io_uring_cqe* cqe = nullptr;
	struct io_uring_sqe* sqe = nullptr;

	while (1) {
		int ret = io_uring_wait_cqe(ring, &cqe);
		if (ret < 0) {
			perror("io_uring_wait_cqe");
			return ret;
		}

		conn_info* info = (conn_info*)io_uring_cqe_get_data(cqe);

		if (info->type == EVENT_ACCEPT) {
			int client_fd = cqe->res;
			printf("New connection: fd=%d, Thread: %d\n", client_fd, std::this_thread::get_id());

			// Prepare read from new client
			info->type = EVENT_READ;
			info->fd = client_fd;

			sqe = io_uring_get_sqe(ring);
			io_uring_prep_recv(sqe, client_fd, info->buffer, sizeof(info->buffer), 0);
			io_uring_sqe_set_data(sqe, info);

		}
		else if (info->type == EVENT_READ) {
			int bytes_read = cqe->res;

			if (bytes_read <= 0) {
				// Connection closed
				printf("Connection closed: fd=%d, Thread: %d\n", info->fd, std::this_thread::get_id());
				close(info->fd);
				delete info;
			}
			else {
				printf("Received %d bytes from fd=%d\n", bytes_read, info->fd);

				// Echo back - prepare write
				sqe = io_uring_get_sqe(ring);
				io_uring_prep_send(sqe, info->fd, info->buffer, bytes_read, 0);
				info->type = EVENT_WRITE;
				io_uring_sqe_set_data(sqe, info);
			}

		}
		else if (info->type == EVENT_WRITE) {
			printf("Sent %d bytes to fd=%d\n", cqe->res, info->fd);

			// Prepare next read
			sqe = io_uring_get_sqe(ring);
			io_uring_prep_recv(sqe, info->fd, info->buffer, sizeof(info->buffer), 0);
			info->type = EVENT_READ;
			io_uring_sqe_set_data(sqe, info);
		}

		io_uring_submit(ring);
		io_uring_cqe_seen(ring, cqe);
	}

	return 0;
}

int main() {
	struct io_uring ring;
	struct io_uring_cqe* cqe;
	struct io_uring_sqe* sqe;

	// Initialize io_uring
	io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

	// Setup listening socket
	int listen_fd = setup_listening_socket(PORT);
	printf("Server listening on port %d\n", PORT);
	fflush(stdout);

	for (int i = 0; i < 10; ++i) {

		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		conn_infos[i].type = EVENT_ACCEPT;

		sqe = io_uring_get_sqe(&ring);

		io_uring_prep_accept(sqe, listen_fd, (struct sockaddr*)&client_addr, &client_len, 0);
		io_uring_sqe_set_data(sqe, &(conn_infos[i]));
	}

	io_uring_submit(&ring);

	std::thread event_threads[10];
	for (int i = 0; i < 10; ++i){
		event_threads[i] = std::thread(worker_thread, listen_fd, &ring);
	}

	for (int i = 0; i < 10; ++i) {
		event_threads[i].join();
	}

	io_uring_queue_exit(&ring);
	close(listen_fd);
	return 0;
}