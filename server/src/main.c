#include "program.h"

int main(int argc, char **argv) {
    Program* program = program_construct();
    int status = program_run(program, argc, argv);
    program_destroy(program);
    return status;
}
