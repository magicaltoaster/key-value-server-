#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "error_helper.h"

#define BUFFER_SIZE 2048

typedef struct receive_messages_args_t {
    int server_fd;
} receive_messages_args_t;

receive_messages_args_t *create_receive_messages_args(int server_fd);

void destroy_receive_messages_args(receive_messages_args_t *args);

/**
 * Handles receiving messages from the thread
 * @param arg
 * @return
 */
void *receive_messages_handler(void *arg);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *ip = argv[1];
    int port = atoi(argv[2]);

    int server_fd;
    struct sockaddr_in server_address;

    /* Socket settings */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    if (connect(server_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        fprintf(stderr, "Error: connection failed.\n");
        return EXIT_FAILURE;
    }
	
    // Start the thread that handles receiving messages
    pthread_t receive_thread;
    receive_messages_args_t* args = create_receive_messages_args(server_fd);
    pthread_create(&receive_thread, NULL, (void *) receive_messages_handler, (void*) args);

    while(1) {
		char *line = NULL;
		size_t len = 0;
		ssize_t line_length = 0;
		line_length = getline(&line, &len, stdin);
        send(server_fd, line, line_length, 0);
		free(line);
    }
    return EXIT_SUCCESS;
}

void *receive_messages_handler(void *arg) {
    receive_messages_args_t * args = (receive_messages_args_t *) arg;
    int server_fd = args->server_fd;
    destroy_receive_messages_args(args);
    char buffer[BUFFER_SIZE];
    int num_bytes;
    int server_active = 1;
    while (server_active) {
        num_bytes = recv(server_fd, buffer, BUFFER_SIZE - 1, 0);
        buffer[num_bytes] = '\0';

        // If there is an error
        if (num_bytes < 0) {
            if(errno == 104) {
                printf("Connection closed by server.\n");
                server_active = 0;
            } else {
                fprintf(stderr, "Error: receiving data.\n");
                print_error();
            }
        }
        // Server shutdown
        else if(num_bytes == 0) {
            printf("Server shutdown.\n");
            server_active = 0;
        }
        else {
            printf("%s", buffer);
        }
    }
    exit(EXIT_SUCCESS);
    pthread_detach(pthread_self());
    return NULL;
}

receive_messages_args_t *create_receive_messages_args(int server_fd) {
    receive_messages_args_t* args = (receive_messages_args_t*)malloc(sizeof(receive_messages_args_t));
    args->server_fd = server_fd;
	return args;
}

void destroy_receive_messages_args(receive_messages_args_t *args) {
    free(args);
}