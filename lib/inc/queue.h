#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

typedef struct Queue {
    List *list;
} Queue;

// Function prototypes
Queue *queue_create();
void queue_destroy(Queue *queue, void (*free_data)(void *));
void queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
size_t queue_size(const Queue *queue);

#endif // QUEUE_H
