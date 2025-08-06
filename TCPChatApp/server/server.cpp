#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 54000
#define BUFFER_SIZE 1024

std::vector<int> client_sockets;
std::unordered_map<int, std::string> client_usernames;
std::mutex clients_mutex;

// Function to handle individual client communication
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // Receive username first
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        std::cerr << "Failed to receive username. Disconnecting client.\n";
        close(client_fd);
        return;
    }

    std::string username(buffer);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_usernames[client_fd] = username;
    }

    std::cout << "Client " << client_fd << " set username: " << username << "\n";

    // Main chat loop
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Client " << username << " disconnected.\n";
            break;
        }

        std::string msg(buffer);

        // Exit command handling
        if (msg.substr(0, 4) == "exit") {
            std::cout << username << " has left the chat.\n";
            break;
        }

        // Print message to server console
        std::cout << "[" << username << "]: " << msg;

        // Broadcast message to other clients
        std::string broadcast_msg = "[" + username + "]: " + msg;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int other_fd : client_sockets) {
            if (other_fd != client_fd) {
                send(other_fd, broadcast_msg.c_str(), broadcast_msg.size(), 0);
            }
        }
    }

    // Cleanup on client disconnect
    close(client_fd);
    clients_mutex.lock();
    client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_fd), client_sockets.end());
    client_usernames.erase(client_fd);
    clients_mutex.unlock();
}

int main() {
    int server_fd;
    sockaddr_in server_addr{}, client_addr{};
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Allow immediate reuse of the port after program exit
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt(SO_REUSEADDR) failed\n";
        close(server_fd);
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "New client connected: " << client_fd << "\n";

        clients_mutex.lock();
        client_sockets.push_back(client_fd);
        clients_mutex.unlock();

        std::thread client_thread(handle_client, client_fd);
        client_thread.detach();
    }

    close(server_fd);
    return 0;
}
