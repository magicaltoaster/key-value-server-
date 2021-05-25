#include "request_message.h"

#include <stdlib.h>

RequestMessage* request_message_construct() {
    RequestMessage* message = (RequestMessage*)malloc(sizeof(RequestMessage));
    message->error = 0;
    message->error_code[0] = '\0';
    message->code[0] = '\0';
    message->length = 0;
    message->payload = NULL;
    return message;
}

void request_message_destroy(RequestMessage* message) {
    if(message->payload != NULL) {
        free(message->payload);
    }
    free(message);
}
