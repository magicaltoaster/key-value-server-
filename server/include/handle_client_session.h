#ifndef HANDLE_CLIENT_SESSION_H
#define HANDLE_CLIENT_SESSION_H

#include "program.h"
#include "client_session.h"

typedef struct HandleClientSessionArgs {
    Program* program;
    ClientSession* session;
} HandleClientSessionArgs;

HandleClientSessionArgs* handle_client_session_args_construct(Program* program, ClientSession* session);
void handle_client_session_args_destroy(HandleClientSessionArgs* args);

void *handle_client_session(void *arg);

#endif
