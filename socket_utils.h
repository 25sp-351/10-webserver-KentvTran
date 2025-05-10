// socket_utils.h
#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int create_server_socket(int port);
int accept_client(int server_fd);
void close_connection(int fd);

#endif
