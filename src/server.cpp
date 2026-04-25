#include "server.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

/*
    server.cpp

    Purpose:
    Implements the basic TCP server setup

    Current milestone:
    - create socket
    - bind socket to a port
    - listen for client connections
    - accept one connection
    - close connection cleanly

    Later:
    - each accepted client will be handled by a separate thread
    - messages will be passed into parser/threat engine/logger modules

*/

Server::Server(int port) : 
    port(port), server_fd(-1), running(false) {}

bool Server::start() {
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "[ERROR] Failed to create socket\n";
        return false;
    }

    // Bind socket to port
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "[ERROR] Failed to set socket options\n";
        close(server_fd);
        return false;
    }

    //Define the server address
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(port);

    // Bind the socket
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        cerr << "[ERROR] Failed to bind socket\n";
        close(server_fd);
        return false;
    }

    // Listen for incoming connections (5 is the backlog size)
    if(listen(server_fd, 5) < 0) {
        cerr << "[ERROR] Failed to listen on socket\n";
        close(server_fd);
        return false;
    }

    running = true;

    cout << "[INFO] Threat Detection Server started.\n";
    cout << "[INFO] Listening on port " << port << "...\n";

    // Accept a single client connection (for now)
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_fd < 0) {
        cerr << "[ERROR] Failed to accept client connection\n";
        stop();
        return false;
    }

    /*
    Convert the client's IP address to a human-readable format 
    */
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    cout << "[INFO] Accepted connection from " << client_ip 
        << ":" << ntohs(client_addr.sin_port) << "\n";
    
    // Close the client connection (for now)
    close(client_fd);
    cout << "[INFO] Client connection closed.\n";
    return true;
}

void Server::stop() {
    //Close the server socket if open 
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
    running = false;
    cout << "[INFO] Threat Detection Server stopped.\n";
}