#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <stdbool.h>
#include <sys/socket.h>

void handle_http(int client_fd);
void* handle_connection_thread(void* arg);

#endif
