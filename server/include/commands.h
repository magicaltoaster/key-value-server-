#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "program.h"
#include "client_session.h"

#define NUM_COMMANDS    3

typedef void (*command_handler)(Program* program, ClientSession* session, char* code, char* payload); 

int route_command(Program* program, ClientSession* session, char* code, char* payload);

void get_handler(Program* program, ClientSession* session, char* code, char* payload);
void set_handler(Program* program, ClientSession* session, char* code, char* payload);
void del_handler(Program* program, ClientSession* session, char* code, char* payload);

int is_command_valid(char* code);

#endif
