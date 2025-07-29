#ifndef MY_LIST_H
#define MY_LIST_H

#include <stddef.h>

// Node structure
typedef struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

// Doubly linked list structure
typedef struct List {
    Node *head;
    Node *tail;
    size_t size;
} List;

// Function prototypes
List *list_create();
void list_destroy(List *list, void (*free_data)(void *));
void list_append(List *list, void *data);
void list_prepend(List *list, void *data);
void *list_remove_head(List *list);
void *list_remove_tail(List *list);
size_t list_size(const List *list);
void *list_find(List *list, const void *data, int (*callback)(void *, const void *)) ;

#endif // MY_LIST_H
