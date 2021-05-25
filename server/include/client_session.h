#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "request_message.h"
#include "response_message.h"

typedef struct ClientSession {
    int socket_fd;
    int active;
} ClientSession;

ClientSession* client_session_construct(int socket_fd);
void client_session_destroy(ClientSession* session);

RequestMessage* client_session_recv_request(ClientSession* session);
void client_session_send_response(ClientSession* session, ResponseMessage* message);
void client_session_close(ClientSession* session);
int client_session_is_active(ClientSession* session);
#endif