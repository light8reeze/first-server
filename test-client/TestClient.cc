#include <arpa/inet.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <unistd.h>
#include <vector>
#include <random>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>


#define SERVER_PORT 4832
#define BUFFER_SIZE 1024

// Must match server definition
struct MessageHeader {
  int size;
  unsigned int message_type;
};

struct TestMessage : public MessageHeader {
  char data[1024];
};

static bool write_all(int fd, const void* data, size_t size) {
  const char* p = static_cast<const char*>(data);
  size_t remaining = size;
  while (remaining > 0) {
    ssize_t sent = send(fd, p, remaining,
#ifdef MSG_NOSIGNAL
                        MSG_NOSIGNAL
#else
                        0
#endif
    );
    if (sent > 0) {
      p += sent;
      remaining -= static_cast<size_t>(sent);
      continue;
    }
    if (sent == 0) {
      return false;
    }
    if (errno == EINTR) {
      continue;
    }
    return false;
  }
  return true;
}

static bool set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return false;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    return false;
  }
  return true;
}

static ssize_t recv_nonblocking(int fd, void* buf, size_t len) {
#ifdef MSG_DONTWAIT
  return recv(fd, buf, len, MSG_DONTWAIT);
#else
  return recv(fd, buf, len, 0);
#endif
}

int main(int argc, char** argv) {
  struct sockaddr_in server_addr;

  // Configure server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  // Convert IPv4 address from text to binary
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    perror("Invalid address");
    return 1;
  }

  long long socket_count = 10000;
  if (argc >= 2) {
    char* end = nullptr;
    long long parsed = strtoll(argv[1], &end, 10);
    if (!end || *end != '\0' || parsed <= 0) {
      std::cerr << "Usage: " << argv[0] << " [socket_count]" << std::endl;
      return 1;
    }
    socket_count = parsed;
  }

  std::vector<int> sockets;
  sockets.reserve(static_cast<size_t>(socket_count));

#ifdef SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#endif

  for (long long i = 0; i < socket_count; ++i) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
      perror("Socket creation failed");
      break;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
      perror("Connection failed");
      close(sock_fd);
      break;
    }

	sockets.push_back(sock_fd);
	std::cout << "Connected sockets: " << (i + 1) << std::endl;
  }

  std::cout << "Total connected sockets: " << sockets.size() << std::endl;

  std::mt19937 rng([] {
    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd()};
    return std::mt19937(seed);
  }());
  std::uniform_int_distribution<int> size_dist(1, 4000);

  while (1) {
      std::vector<char> out;
      uint64_t msg_seq = 0;
      for (size_t i = 0; i < sockets.size(); ++i) {
          int payload_size = size_dist(rng);
          MessageHeader hdr;
          hdr.size = static_cast<int>(sizeof(MessageHeader)) + payload_size;
          hdr.message_type = 1;

          out.resize(static_cast<size_t>(hdr.size));
          memcpy(out.data(), &hdr, sizeof(hdr));
          memset(out.data() + sizeof(hdr), 'a' + static_cast<char>(i % 26),
              static_cast<size_t>(payload_size));

          std::string prefix =
              "client=" + std::to_string(i) + "; msg=" +
              std::to_string(static_cast<unsigned long long>(msg_seq++)) +
              "; payload=" + std::to_string(payload_size) + "; ";
          size_t prefix_copy =
              std::min(prefix.size(), static_cast<size_t>(payload_size));
          memcpy(out.data() + sizeof(hdr), prefix.data(), prefix_copy);

          if (!write_all(sockets[i], out.data(), out.size())) {
              perror("Send failed");
              close(sockets[i]);
              sockets[i] = -1;
          }

          std::cout << "Sent initial messages: " << (i + 1) << std::endl;
      }
  }

  sockets.erase(std::remove(sockets.begin(), sockets.end(), -1), sockets.end());
  std::cout << "Ready to receive on sockets: " << sockets.size() << std::endl;

  int epfd = epoll_create1(0);
  if (epfd < 0) {
    perror("epoll_create1 failed");
    return 1;
  }

  for (int fd : sockets) {
    if (!set_nonblocking(fd)) {
      perror("set_nonblocking failed");
      close(fd);
      continue;
    }

    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
      perror("epoll_ctl add failed");
      close(fd);
      continue;
    }
  }

  std::cout << "Receiving forever (Ctrl+C to stop)" << std::endl;

  constexpr int kMaxEvents = 1024;
  std::vector<epoll_event> events(static_cast<size_t>(kMaxEvents));
  std::vector<char> inbuf(8192);

  uint64_t total_bytes = 0;
  uint64_t total_reads = 0;
  auto last_report = std::chrono::steady_clock::now();

  for (;;) {
    int n = epoll_wait(epfd, events.data(), kMaxEvents, 1000);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      perror("epoll_wait failed");
      break;
    }

    for (int i = 0; i < n; ++i) {
      int fd = events[i].data.fd;
      uint32_t ev = events[i].events;

      if (ev & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
        close(fd);
        continue;
      }

      if (!(ev & EPOLLIN)) {
        continue;
      }

      for (;;) {
        ssize_t r = recv_nonblocking(fd, inbuf.data(), inbuf.size());
        if (r > 0) {
          total_bytes += static_cast<uint64_t>(r);
          ++total_reads;
          continue;
        }
        if (r == 0) {
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
          close(fd);
          break;
        }
        if (errno == EINTR) {
          continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          break;
        }
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
        close(fd);
        break;
      }
    }

    auto now = std::chrono::steady_clock::now();
    if (now - last_report >= std::chrono::seconds(1)) {
      std::cout << "recv: " << total_reads << " reads, " << total_bytes
                << " bytes" << std::endl;
      last_report = now;
    }
  }

  close(epfd);

  return 0;
}
