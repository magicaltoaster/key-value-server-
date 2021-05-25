#include "error_helper.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

void print_error() {
    fprintf(stderr, "Error code: %d\n", errno);
    fprintf(stderr, "Error description: %s\n", strerror(errno));
}