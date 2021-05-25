#ifndef REQUEST_MESSAGE_H
#define REQUEST_MESSAGE_H

#include "constants.h"

typedef struct RequestMessage {
    int error;
    char error_code[ERROR_CODE_LENGTH + 1];
    char code[REQUEST_CODE_LENGTH + 1];
    int length;
    char* payload;
} RequestMessage;

RequestMessage* request_message_construct();
void request_message_destroy(RequestMessage* message);
#endif
