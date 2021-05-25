#ifndef RESPONSE_MESSAGE_H
#define RESPONSE_MESSAGE_H

#include "constants.h"

typedef struct ResponseMessage {
    int error;
    char code[RESPONSE_CODE_LENGTH + 1];
    int length;
    char* payload;
} ResponseMessage;

ResponseMessage* response_message_construct();
void response_message_destroy(ResponseMessage* message);
#endif
