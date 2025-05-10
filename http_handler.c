#include "http_handler.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "response.h"
#include "socket_utils.h"

#define STATIC_DIR "static"
#define MAX_PATH_LEN 1024       // max length of paths
#define FIXED_BUFFER_SIZE 1024  // fixed buffer size for responses

extern int verbose;

void handle_static(int client_fd, const char* path) {
    // constructs path
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", STATIC_DIR, path);

    if (strstr(path, "..") != NULL) {
        reply_error(client_fd, HTTP_400, "Invalid path");
        return;
    }

    FILE* file = fopen(full_path, "rb");
    if (!file) {
        reply_error(client_fd, HTTP_404, "File not found");
        return;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(file_size);
    if (!buffer) {
        reply_error(client_fd, HTTP_500, "Internal Server Error");
        fclose(file);
        return;
    }

    if (fread(buffer, 1, file_size, file) != file_size) {
        free(buffer);
        fclose(file);
        reply_error(client_fd, HTTP_500, "Read error");
        return;
    }
    fclose(file);

    send_response(client_fd, HTTP_200, get_content_type(full_path), buffer,
                  file_size);
    free(buffer);
}

void handle_calc(int client_fd, const char* op, const char* num1_str,
                 const char* num2_str) {
    char *endptr1, *endptr2;
    long num1, num2;
    int result;

    // convert/check first number is a number and within range
    errno = 0;
    num1  = strtol(num1_str, &endptr1, 10);
    if (endptr1 == num1_str || *endptr1 != '\0' || errno == ERANGE ||
        num1 < INT_MIN || num1 > INT_MAX) {
        reply_error(client_fd, HTTP_400, "Invalid numbers");
        return;
    }

    // convert/check second number is a number and within range
    errno = 0;
    num2  = strtol(num2_str, &endptr2, 10);
    if (endptr2 == num2_str || *endptr2 != '\0' || errno == ERANGE ||
        num2 < INT_MIN || num2 > INT_MAX) {
        reply_error(client_fd, HTTP_400, "Invalid numbers");
        return;
    }
    // checks operation and performs/stores it to result
    if (strcmp(op, "add") == 0)
        result = (int)num1 + (int)num2;
    else if (strcmp(op, "mul") == 0)
        result = (int)num1 * (int)num2;
    else if (strcmp(op, "div") == 0) {
        if (num2 == 0) {
            reply_error(client_fd, HTTP_400, "Division by zero");
            return;
        }
        result = (int)num1 / (int)num2;
    } else {
        reply_error(client_fd, HTTP_404, "Invalid operation");
        return;
    }

    // generate HTML response with result
    char body[FIXED_BUFFER_SIZE];
    int body_len =
        snprintf(body, sizeof(body),
                 "<html><body><h1>Result: %d</h1></body></html>", result);
    send_response(client_fd, HTTP_200, "text/html", body, body_len);
}

void handle_http_request(int client_fd, const char* method, const char* path) {
    // log request if verbose is enabled
    if (verbose)
        printf("Request: %s %s\n", method, path);

    // route to static
    if (strncmp(path, "/static/", LITERAL_LEN("/static/")) == 0) {
        handle_static(client_fd, path + LITERAL_LEN("/static/"));
    }
    // route to calc
    else if (strncmp(path, "/calc/", LITERAL_LEN("/calc/")) == 0) {
        char op[16], num1[32], num2[32];
        if (sscanf(path + LITERAL_LEN("/calc/"),
                   "%15[^/]/%31[^/]/%31s",  // 15 is max length of operation, 31 is max length of num1 and num2
                   op, num1, num2) != 3) { // expects 3 arguments
            reply_error(client_fd, HTTP_400, "Invalid calc path");
            return;
        }
        handle_calc(client_fd, op, num1, num2);
    } else if (strncmp(path, "/calc/", LITERAL_LEN("/calc/")) == 0) {
        char op[16], num1[32], num2[32];
        if (sscanf(path + LITERAL_LEN("/calc/"),
                   "%15[^/]/%31[^/]/%31s",  // 15 is max length of operation, 31 is max length of num1 and num2
                   op, num1, num2) != 3) {  // expects 3 arguments
            reply_error(client_fd, HTTP_400, "Invalid calc path");
            return;
        }
        handle_calc(client_fd, op, num1, num2);
    } else {
        reply_error(client_fd, HTTP_404, "Not found");
    }
}
