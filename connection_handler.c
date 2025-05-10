#include "connection_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "http_handler.h"
#include "request.h"
#include "response.h"
#include "socket_utils.h"

extern int verbose;

void handle_http(int client_fd) {
    Request* req = parse_request(client_fd);
    if (!req) {
        close_connection(client_fd);
        return;
    }

    // only handles GET requests
    if (strcmp(req->method, "GET") != 0) {
        reply_error(client_fd, HTTP_405, "Method not allowed");
        free_request(req);
        close_connection(client_fd);
        return;
    }

    handle_http_request(client_fd, req->method, req->path);

    free_request(req);
    close_connection(client_fd);
}

void* handle_connection_thread(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);
    handle_http(client_fd);
    return NULL;
}
