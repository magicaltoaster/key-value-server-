#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "response_codes.h"
#include "error_codes.h"

#define UNUSED(x) (void)(x)

char* command_codes[NUM_COMMANDS] = {
    "GET",
    "SET",
    "DEL"
};

command_handler command_handlers[NUM_COMMANDS] = {
    &get_handler,
    &set_handler,
    &del_handler
};

int is_command_valid(char* code) {
    int i;
    for(i = 0; i < NUM_COMMANDS; i++) {
        if(strcmp(code, command_codes[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int route_command(Program* program, ClientSession* session, char* code, char* payload) {
    UNUSED(code);
    int i;
    for(i = 0; i < NUM_COMMANDS; i++) {
        if(strcmp(code, command_codes[i]) == 0) {
            command_handlers[i](program, session, code, payload);
            return 1;
        }
    }
    return 0;
}

void get_handler(Program* program, ClientSession* session, char* code, char* payload) {
    UNUSED(code);
    payload[strlen(payload) - 1] = '\0';
    char* key = payload;
    char* value = key_value_service_get(program->service, key);
    ResponseMessage* response = response_message_construct();
    if(value == NULL) {
        strcpy(response->code, RESPONSE_CODE_KNF);
    } else {
        strcpy(response->code, RESPONSE_CODE_OKG);
        response->length = strlen(value) + 1;
        response->payload = (char*)malloc(sizeof(char) * (response->length + 1));
        sprintf(response->payload, "%s\n", value);
        free(value);
    }
    client_session_send_response(session, response);
    response_message_destroy(response);
}

void set_handler(Program* program, ClientSession* session, char* code, char* payload) {
    UNUSED(code);
    char* delimiter = "\n";
    payload[strlen(payload) - 1] = '\0';
    char* key = strtok(payload, delimiter);
    char* value = strtok(NULL, delimiter);

    key_value_service_put(program->service, key, value);
    ResponseMessage* response = response_message_construct();
    strcpy(response->code, RESPONSE_CODE_OKS);
    client_session_send_response(session, response);
    response_message_destroy(response);
}

void del_handler(Program* program, ClientSession* session, char* code, char* payload) {
    UNUSED(code);
    payload[strlen(payload) - 1] = '\0';
    char* key = payload;
    char* value = key_value_service_get(program->service, key);
    ResponseMessage* response = response_message_construct();
    if(value == NULL) {
        strcpy(response->code, RESPONSE_CODE_KNF);
    } else {
        strcpy(response->code, RESPONSE_CODE_OKD);
        response->length = strlen(value) + 1;
        response->payload = (char*)malloc(sizeof(char) * (response->length + 1));
        sprintf(response->payload, "%s\n", value);
        free(value);
        key_value_service_remove(program->service, key);
    }
    client_session_send_response(session, response);
    response_message_destroy(response);    
}

