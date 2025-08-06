#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>

#define PORT 54000
#define BUFFER_SIZE 1024

// ANSI color codes for red text and reset
#define RED "\033[31m"
#define RESET "\033[0m"

std::atomic<bool> running(true);

void receive_messages(int sock_fd) {
    char buffer[BUFFER_SIZE];
    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "\nServer disconnected.\n";
            running = false;
            break;
        }
        // Print incoming message and reprint prompt with [You] in red, no extra blank lines
        std::cout << "\r" << buffer << RED << "[You]: " << RESET << std::flush;
    }
}

int main() {
    int sock_fd;
    sockaddr_in server_addr{};

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        close(sock_fd);
        return 1;
    }

    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        close(sock_fd);
        return 1;
    }

    std::cout << "Connected to Server\n";

    std::cout << "Enter your username: ";
    std::string username;
    std::getline(std::cin, username);
    send(sock_fd, username.c_str(), username.length(), 0);

    std::thread recv_thread(receive_messages, sock_fd);

    while (running) {
        std::cout << RED << "[You]: " << RESET;
        std::string msg;
        std::getline(std::cin, msg);
        msg += '\n';

        send(sock_fd, msg.c_str(), msg.length(), 0);

        if (msg.substr(0, 4) == "exit") {
            running = false;
            break;
        }
    }

    recv_thread.join();
    close(sock_fd);
    return 0;
}
