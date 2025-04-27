#ifndef RESPONSE_H
#define RESPONSE_H

#include <unistd.h>

#define LITERAL_LEN(x) (sizeof(x) - 1)

extern const char* HTTP_200;
extern const char* HTTP_400;
extern const char* HTTP_404;
extern const char* HTTP_500;
extern const char* HTTP_405;

const char* get_content_type(const char* path);
void send_response(int fd, const char* status, const char* content_type,
                   const char* body, int content_length);
void reply_error(int fd, const char* code, const char* msg);

#endif
