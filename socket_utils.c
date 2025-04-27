#include "socket_utils.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 3  // maximum number of pending connections in the queue

// creates and configure TCP server socket bound to a port
int create_server_socket(int port) {
    int socket_fd;
    struct sockaddr_in address;

    // Create socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure socket address
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port        = htons(port);

    // Bind socket to the given port
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening using a defined backlog
    if (listen(socket_fd, BACKLOG) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Binding to port %d\n", port);
    return socket_fd;
}

int accept_client(int socket_fd) {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    printf("Waiting for connection...\n");
    if ((client_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
                            &addr_len)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");
    return client_fd;
}

void close_connection(int fd) {
    close(fd);
}
