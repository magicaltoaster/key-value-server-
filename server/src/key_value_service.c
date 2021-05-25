#include "key_value_service.h"

#include <stdlib.h>
#include <string.h>

LinkedListNode* linked_list_node_construct(char* key, char* value) {
    LinkedListNode* node = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    
    node->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(node->key, key);

    node->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(node->value, value);

    node->next= NULL;
    return node;
}

void linked_list_node_destroy(LinkedListNode* node) {
    if(node->key != NULL) {
        free(node->key);
    }

    if(node->value != NULL) {
        free(node->value);
    }

    free(node);
}

KeyValueService* key_value_service_construct() {
    KeyValueService* service = (KeyValueService*)malloc(sizeof(KeyValueService));
    service->root = NULL;
    return service;
}

void key_value_service_destroy(KeyValueService* service) {
    LinkedListNode* node = service->root;
    while(node != NULL) {
        LinkedListNode* temp = node;
        node = node->next;
        linked_list_node_destroy(temp);
    }
    free(service);
}

LinkedListNode* _key_value_service_find_node(KeyValueService* service, char* key) {
    LinkedListNode* node = service->root;
    while(node != NULL && strcmp(key, node->key) > 0) {
        node = node->next;
    }
    
    if(node != NULL && strcmp(key, node->key) == 0) {
        return node;
    }
    return NULL;
}

char* key_value_service_get(KeyValueService* service, char* key) {
    char* value = NULL;
    pthread_mutex_lock(&service->mutex);
    LinkedListNode* node = _key_value_service_find_node(service, key);
    if(node != NULL) {
        value = (char*)calloc(sizeof(char), strlen(node->value) + 1);
        strcpy(value, node->value);
    } 
    pthread_mutex_unlock(&service->mutex);
    return value;
}

void key_value_service_put(KeyValueService* service, char* key, char* value) {
    pthread_mutex_lock(&service->mutex);
    LinkedListNode* previous_node = NULL;
    LinkedListNode* node = service->root;

    if(node != NULL) {
        while(strcmp(key, node->key) > 0) {
            previous_node = node;
            node = node->next;
        }

        if(strcmp(key, node->key) == 0) {
            pthread_mutex_unlock(&service->mutex);
            return;
        }
    }

    LinkedListNode* new_node = linked_list_node_construct(key, value);
    if(previous_node == NULL) {
        new_node->next = service->root;
        service->root = new_node;
        pthread_mutex_unlock(&service->mutex);
        return;
    }
    previous_node->next = new_node;
    new_node->next = node;
    pthread_mutex_unlock(&service->mutex);
}

void key_value_service_remove(KeyValueService* service, char* key) {
    if(service->root == NULL) {
        return;
    }

    pthread_mutex_lock(&service->mutex);
    LinkedListNode* previous_node = NULL;
    LinkedListNode* node = service->root;

    while(strcmp(key, node->key) > 0) {
        previous_node = node;
        node = node->next;
    }

    if(strcmp(key, node->key) == 0) {
        LinkedListNode* temp = node;
        if(previous_node != NULL) {
            previous_node->next = node->next;
        }
        else {
            service->root = node->next;
        }
        linked_list_node_destroy(temp);
    }
    pthread_mutex_unlock(&service->mutex);
}