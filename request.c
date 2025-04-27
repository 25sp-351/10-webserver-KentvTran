#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096  // Buffer size for reading request data

static bool parse_request_line(char *buffer, Request *req) {
    char *saveptr;
    // extract method
    char *token = strtok_r(buffer, " \t\n\r", &saveptr);
    if (!token)
        return false;
    req->method = strdup(token);

    // extract path
    token = strtok_r(NULL, " \t\n\r", &saveptr);
    if (!token)
        return false;
    req->path = strdup(token);

    // extract protocol version
    token = strtok_r(NULL, "\r\n", &saveptr);
    if (!token)
        return false;
    req->protocol = strdup(token);

    return true;
}
// parse headers to a list
static void parse_headers(char *buffer, Request *req) {
    char *line, *saveptr;
    req->headers  = NULL;
    Header **tail = &req->headers;

    line          = strtok_r(buffer, "\r\n", &saveptr);  // Skip request line
    line          = strtok_r(NULL, "\r\n", &saveptr);

    while (line && *line) {
        char *colon = strchr(line, ':');
        if (!colon)
            break;

        *colon      = '\0';
        char *key   = line;
        char *value = colon + 1;
        while (*value == ' ')
            value++;

        Header *h = malloc(sizeof(Header));
        h->key    = strdup(key);
        h->value  = strdup(value);
        h->next   = NULL;

        *tail     = h;
        tail      = &h->next;

        line      = strtok_r(NULL, "\r\n", &saveptr);
    }
}

// main function to parse the request
Request *parse_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    // Read the request data from the client socket
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
        return NULL;
    buffer[bytes_read] = '\0';

    Request *req       = calloc(1, sizeof(Request));
    if (!parse_request_line(buffer, req)) {
        free_request(req);
        return NULL;
    }

    parse_headers(buffer, req);
    return req;
}

void free_request(Request *req) {
    if (!req)
        return;
    free(req->method);
    free(req->path);
    free(req->protocol);

    Header *h = req->headers;
    while (h) {
        Header *tmp = h;
        h           = h->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    free(req);
}

const char *get_header_value(const Request *req, const char *key) {
    for (Header *h = req->headers; h; h = h->next)
        if (strcasecmp(h->key, key) == 0)
            return h->value;
    return NULL;
}
