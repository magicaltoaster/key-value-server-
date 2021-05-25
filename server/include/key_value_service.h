#ifndef KEY_VALUE_SERVICE_H
#define KEY_VALUE_SERVICE_H

#include <pthread.h>

typedef struct LinkedListNode {
    char* key;
    char* value;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct KeyValueService {
    LinkedListNode* root;
    pthread_mutex_t mutex;
} KeyValueService;

LinkedListNode* linked_list_node_construct(char* key, char* value);
void linked_list_node_destroy(LinkedListNode* node);

KeyValueService* key_value_service_construct();
void key_value_service_destroy(KeyValueService* service);

LinkedListNode* _key_value_service_find_node(KeyValueService* service, char* key);

char* key_value_service_get(KeyValueService* service, char* key);
void key_value_service_put(KeyValueService* service, char* key, char* value);
void key_value_service_remove(KeyValueService* service, char* key);

#endif