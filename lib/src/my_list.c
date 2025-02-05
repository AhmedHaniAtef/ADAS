#include <stdlib.h>
#include "../inc/my_list.h"

// Create a new list
List *list_create() {
    List *list = (List *)malloc(sizeof(List));
    if (!list) return NULL;
    list->head = list->tail = NULL;
    list->size = 0;
    return list;
}

// Destroy the list
void list_destroy(List *list, void (*free_data)(void *)) {
    if (!list) return;

    Node *current = list->head;
    while (current) {
        Node *next = current->next;
        if (free_data) free_data(current->data);
        free(current);
        current = next;
    }
    free(list);
}

// Append an element to the end of the list
void list_append(List *list, void *data) {
    if (!list) return;

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = list->tail;

    if (list->tail) list->tail->next = new_node;
    list->tail = new_node;

    if (!list->head) list->head = new_node;
    list->size++;
}

// Prepend an element to the start of the list
void list_prepend(List *list, void *data) {
    if (!list) return;

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = list->head;

    if (list->head) list->head->prev = new_node;
    list->head = new_node;

    if (!list->tail) list->tail = new_node;
    list->size++;
}

// Remove and return the head element
void *list_remove_head(List *list) {
    if (!list || !list->head) return NULL;

    Node *node = list->head;
    void *data = node->data;

    list->head = node->next;
    if (list->head) list->head->prev = NULL;
    else list->tail = NULL;

    free(node);
    list->size--;
    return data;
}

// Remove and return the tail element
void *list_remove_tail(List *list) {
    if (!list || !list->tail) return NULL;

    Node *node = list->tail;
    void *data = node->data;

    list->tail = node->prev;
    if (list->tail) list->tail->next = NULL;
    else list->head = NULL;

    free(node);
    list->size--;
    return data;
}

// Get the size of the list
size_t list_size(const List *list) {
    return list ? list->size : 0;
}

// Find data based on a callback function
void *list_find(List *list, const void *data, int (*callback)(void *, const void *)) {
    if (!list || !callback) return NULL;

    Node *current = list->head;
    while (current) {
        if (callback(current->data, data)) {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}
