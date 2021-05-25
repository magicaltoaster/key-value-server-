#include "client_session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "constants.h"
#include "error_codes.h"
#include "error_helper.h"
#include "commands.h"

#define BUFFER_SIZE 64

ClientSession* client_session_construct(int socket_fd) {
    ClientSession* session = (ClientSession*)malloc(sizeof(ClientSession));
    session->socket_fd = socket_fd;
    session->active = 1;
    return session;
}

void client_session_destroy(ClientSession* session) {
    if(session != NULL) {
        free(session);
    }
}


RequestMessage* incorrect_length_message() {
    RequestMessage* request = request_message_construct();
    request->error = 1;
    strcpy(request->error_code, ERROR_INCORRECT_LENGTH);
    return request;
}

RequestMessage* malformed_message() {
    RequestMessage* request = request_message_construct();
    request->error = 1;
    strcpy(request->error_code, ERROR_MALFORMED_MESSAGE);
    return request;
}

RequestMessage* client_session_recv_request(ClientSession* session) {
    char buffer[BUFFER_SIZE];
    char code[REQUEST_CODE_LENGTH + 1];
    
    char* payload;
    char payload_length_str[12];
    int payload_length = -1;
    int payload_read = 0;

    int num_bytes = recv(session->socket_fd, code, REQUEST_CODE_LENGTH + 1, MSG_WAITALL);
    if(num_bytes < 0) {
        print_error();
    }
    if(num_bytes <= 0) {
        session->active = 0;
        return NULL;
    }

    RequestMessage* request;
    if(code[REQUEST_CODE_LENGTH] != '\n') {
        return malformed_message();
    }
    code[REQUEST_CODE_LENGTH] = '\0';
    
    if(!is_command_valid(code)) {
        return malformed_message();
    }
    num_bytes = recv(session->socket_fd, payload_length_str, 12, 0);
    if(num_bytes < 0) {
        print_error();
    }
    if(num_bytes <= 0) {
        return NULL;
    }

    sscanf(payload_length_str, "%d", &payload_length);
    if(payload_length < 0) {
        return malformed_message();
    }
    payload = (char*)calloc(sizeof(char), payload_length + 1);

    while(payload_read < payload_length) {
        num_bytes = recv(session->socket_fd, buffer, BUFFER_SIZE, 0);
        strcpy(payload + payload_read, buffer);
        payload_read += num_bytes;
    }
    payload[payload_length] = '\0';
    if(payload[payload_length - 1] != '\n') {
        free(payload);
        return incorrect_length_message();
    }
    request = request_message_construct();
    strcpy(request->code, code);
    request->length = payload_length;
    request->payload = payload;
    return request; 
}

void client_session_send_response(ClientSession* session, ResponseMessage* message) {
    if(message->error) {
        send(session->socket_fd, "ERR", 3, 0);
        send(session->socket_fd, "\n", 1, 0);
    }
    send(session->socket_fd, message->code, RESPONSE_CODE_LENGTH, 0);
    send(session->socket_fd, "\n", 1, 0);
    if(message->length > 0) {
        char payload_length[12];
        sprintf(payload_length, "%d", message->length);
        send(session->socket_fd, payload_length, strlen(payload_length), 0);
        send(session->socket_fd, "\n", 1, 0);
        send(session->socket_fd, message->payload, strlen(message->payload), 0);
    }
}

void client_session_close(ClientSession* session) {
    close(session->socket_fd);
    session->active = 0;
}

int client_session_is_active(ClientSession* session) {
    return session->active;
}