#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

#define SERVER_PORT 4832
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock_fd);
        return 1;
    }

    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock_fd);
        return 1;
    }

    std::cout << "Connected to server at 127.0.0.1:" << SERVER_PORT << std::endl;

    // Test messages to send
    const char* test_messages[] = {
        "Hello, Server!",
        "This is a test message.",
        "Echo test 123",
        "Final message"
    };

    // Send and receive test messages
    for (int i = 0; i < 4; i++) {
        const char* message = test_messages[i];
        int message_len = strlen(message);

        std::cout << "\n[Sending] " << message << std::endl;

        // Send message
        ssize_t sent = send(sock_fd, message, message_len, 0);
        if (sent < 0) {
            perror("Send failed");
            break;
        }

        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Receive echo
        ssize_t received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (received < 0) {
            perror("Receive failed");
            break;
        } else if (received == 0) {
            std::cout << "Server closed connection" << std::endl;
            break;
        }

        buffer[received] = '\0';
        std::cout << "[Received] " << buffer << " (" << received << " bytes)" << std::endl;

        // Small delay between messages
        usleep(100000); // 100ms
    }

    // Close connection
    close(sock_fd);
    std::cout << "\nConnection closed" << std::endl;

    return 0;
}
