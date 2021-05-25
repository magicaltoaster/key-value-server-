#ifndef PROGRAM_H
#define PROGRAM_H

#include "key_value_service.h"

typedef struct Program {
    KeyValueService* service;
} Program;

Program* program_construct();
void program_destroy(Program* program);

int program_run(Program* program, int argc, char **argv);
#endif