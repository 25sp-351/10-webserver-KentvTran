#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <unistd.h>

#define STATIC_DIR "static"
#define MAX_PATH_LEN 1024
#define FIXED_BUFFER_SIZE 1024

void handle_http_request(int client_fd, const char* method, const char* path);
void serve_static(int client_fd, const char* path);
void handle_calc(int client_fd, const char* op, const char* num1,
                 const char* num2);

#endif
