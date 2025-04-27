#include "response.h"

#include <stdio.h>
#include <string.h>

#define FIXED_BUFFER_SIZE 1024  // Fixed buffer size for header

const char* HTTP_200 = "HTTP/1.1 200 OK\r\n";
const char* HTTP_400 = "HTTP/1.1 400 Bad Request\r\n";
const char* HTTP_404 = "HTTP/1.1 404 Not Found\r\n";
const char* HTTP_500 = "HTTP/1.1 500 Internal Server Error\r\n";
const char* HTTP_405 = "HTTP/1.1 405 Method Not Allowed\r\n";

const char* get_content_type(const char* path) {
    const char* dot = strrchr(path, '.');  // Find the last dot in the path
    if (!dot)
        return "text/plain";

    if (strcmp(dot, ".html") == 0)
        return "text/html";
    if (strcmp(dot, ".txt") == 0)
        return "text/plain";
    if (strcmp(dot, ".jpg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    return "text/plain";  // Default content type
}

void reply_error(int fd, const char* code, const char* msg) {
    send_response(fd, code, "text/plain", msg, (int)strlen(msg));
}

void send_response(int fd, const char* status, const char* content_type,
                   const char* body, int content_length) {
    // construct response header
    char header[FIXED_BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
                              "%s"  // status line
                              "Content-Type: %s\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n\r\n",
                              status, content_type, content_length);

    // send header and body
    ssize_t sent = write(fd, header, header_len);
    // checks Handle partial write
    if (sent != header_len) {
        fprintf(stderr, "Partial header write: %zd/%d bytes\n", sent,
                header_len);
    }
    sent = write(fd, body, content_length);
}
