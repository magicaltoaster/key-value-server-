#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#include "client_session.h"
#include "error_helper.h"
#include "handle_client_session.h"

int is_valid_port(int port);

Program* program_construct() {
    Program* program = (Program*)malloc(sizeof(Program));
    program->service = key_value_service_construct();
    return program;
}

void program_destroy(Program* program) {
    key_value_service_destroy(program->service);
    free(program);
}

int program_run(Program* program, int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    if (!is_valid_port(port) || port == 0) {
        fprintf(stderr, "Error: port range is between 1 and 65535.\n");
        return EXIT_FAILURE;
    }

    int server_fd;
    struct sockaddr_in server_address;

    int client_fd;

    /* Creating socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "Error: socket creation failed.\n");
        return EXIT_FAILURE;
    }

    /* Socket settings */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    /* Socket options */
    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), &enable, sizeof(enable)) < 0) {
        fprintf(stderr, "Error: setsockopt failed.\n");
        return EXIT_FAILURE;
    }

    /* Binding */
    if (bind(server_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        fprintf(stderr, "Error: binding failed.\n");
        print_error();
        return EXIT_FAILURE;
    }

    /* Start listening */
    if (listen(server_fd, 10) < 0) {
        fprintf(stderr, "Error: listening failed.\n");
        print_error();
        return EXIT_FAILURE;
    }
	
    printf("Started listening...\n");

    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *) NULL, NULL)) < 0) {
            fprintf(stderr, "Error: accepting client failed.\n");
            print_error();
            continue;
        }

		ClientSession* session = client_session_construct(client_fd);
        pthread_t thread;
        HandleClientSessionArgs* args = handle_client_session_args_construct(program, session);
        pthread_create(&thread, NULL, &handle_client_session, (void *) args);
    }
    return EXIT_SUCCESS;
}

int is_valid_port(int port) {
    return port >= 0 && port <= 65535;
}