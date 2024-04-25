#include <thread>
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <json/json.h>
#include <iostream>
#include "include/server.h"
#include "include/handler.h"

std::mutex file_mutex;

void process_client(int client_socket) {
    char buffer[4096];
    read(client_socket, buffer, 4096);
    std::string request_str(buffer);

    Json::Reader reader;
    Json::Value request;
    reader.parse(request_str, request);

    std::string action = request["action"].asString();

    Json::Value response;

    std::lock_guard<std::mutex> lock(file_mutex);


    if (action == "add") {
        response.append(add_new_entry(request));
    }
    else if (action == "delete") {
        response.append(delete_entry(request));
    }
    else if (action == "search") {
        response.append(search_entry(request));
    }
    else if (action == "view") {
        response.append(get_all_entry());
    }

    std::string response_str = response.toStyledString();
    write(client_socket, response_str.c_str(), response_str.length());
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    int enable = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
        close(server_fd);
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind" << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        close(server_fd);
        return 1;
    }

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }
        std::thread client_thread(process_client, new_socket);
        client_thread.detach();
    }

    return 0;
}