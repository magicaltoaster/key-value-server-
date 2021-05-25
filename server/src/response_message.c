#include "response_message.h"

#include <stdlib.h>

ResponseMessage* response_message_construct() {
    ResponseMessage* message = (ResponseMessage*)malloc(sizeof(ResponseMessage));
    message->error = 0;
    message->code[0] = '\0';
    message->length = 0;
    message->payload = NULL;
    return message;
}

void response_message_destroy(ResponseMessage* message) {
    if(message->payload != NULL) {
        free(message->payload);
    }
    free(message);
}
