#include "handle_client_session.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "commands.h"

HandleClientSessionArgs* handle_client_session_args_construct(Program* program, ClientSession* session) {
    HandleClientSessionArgs* args = (HandleClientSessionArgs*)malloc(sizeof(HandleClientSessionArgs));
    args->program = program;
    args->session = session;
    return args;
}

void handle_client_session_args_destroy(HandleClientSessionArgs* args) {
    if(args != NULL) {
        free(args);
    }
}

void *handle_client_session(void *arg) {
    HandleClientSessionArgs* args = (HandleClientSessionArgs*)arg;
    Program* program = args->program;
	ClientSession* session = args->session;
    handle_client_session_args_destroy(args);

    while(client_session_is_active(session)) {
        RequestMessage* request = client_session_recv_request(session);
        if(request != NULL) {
            if(request->error) {
                ResponseMessage* response = response_message_construct();
                response->error = 1;
                strcpy(response->code, request->error_code);
                client_session_send_response(session, response);
                response_message_destroy(response);
                client_session_close(session);
            } else {
                route_command(program, session, request->code, request->payload);
            }
            request_message_destroy(request);
        }
    }
    pthread_detach(pthread_self());
    return NULL;
}
