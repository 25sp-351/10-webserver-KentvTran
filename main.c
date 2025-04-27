#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "connection_handler.h"
#include "http_handler.h"
#include "response.h"
#include "socket_utils.h"

#define DEFAULT_PORT 80
#define MAX_PORT 65535

int socket_fd;

// Global switch to print received texts to terminal, default is off
int verbose = 0;

// handles SIGINT signal to close server
void sigint_handler(int sig) {
    (void)sig;
    close_connection(socket_fd);
    printf("\nServer shutdown\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;

    // Process command-line arguments: -p <port_value> and -v
    for (int ix = 1; ix < argc; ix++) {
        if (strcmp(argv[ix], "-p") == 0) {
            if (ix + 1 < argc) {
                char* endptr;
                long p =
                    strtol(argv[ix + 1], &endptr,
                           10);  // 10 is to treat arg as base-10/decimal value
                if (*endptr == '\0' && p > 0 && p <= MAX_PORT) {
                    port = (int)p;
                    ix++;
                } else {
                    fprintf(stderr, "Invalid port number: %s\n", argv[ix + 1]);
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "Option -p requires an argument.\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[ix], "-v") == 0) {
            verbose = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[ix]);
            exit(EXIT_FAILURE);
        }
    }

    signal(SIGINT, sigint_handler);
    socket_fd = create_server_socket(port);

    while (1) {
        int client_fd      = accept_client(socket_fd);

        int* client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr     = client_fd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_connection_thread,
                           client_fd_ptr) != 0) {
            perror("Thread creation failed");
            free(client_fd_ptr);
            close_connection(client_fd);
        }
        pthread_detach(thread);
    }

    return 0;
}
