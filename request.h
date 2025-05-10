#ifndef REQUEST_H
#define REQUEST_H

#include <stdbool.h>
#include <sys/types.h>

typedef struct Header {
    char *key;
    char *value;
    struct Header *next;
} Header;

typedef struct {
    char *method;
    char *path;
    char *protocol;
    Header *headers;
} Request;

Request *parse_request(int client_fd);
void free_request(Request *req);

const char *get_header_value(const Request *req, const char *key);

#endif
