#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "my_list.h"

typedef struct Queue {
    List *list;
} Queue;

// Function prototypes
Queue *queue_create();
void queue_destroy(Queue *queue, void (*free_data)(void *));
void queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
size_t queue_size(const Queue *queue);

#endif // MY_QUEUE_H
