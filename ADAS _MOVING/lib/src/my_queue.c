#include <stdlib.h>
#include "../inc/my_queue.h"

// Create a new queue
Queue *queue_create() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->list = list_create();
    if (!queue->list) {
        free(queue);
        return NULL;
    }
    return queue;
}

// Destroy the queue
void queue_destroy(Queue *queue, void (*free_data)(void *)) {
    if (!queue) return;

    list_destroy(queue->list, free_data);
    free(queue);
}

// Enqueue an element
void queue_enqueue(Queue *queue, void *data) {
    if (!queue) return;

    list_append(queue->list, data);
}

// Dequeue an element
void *queue_dequeue(Queue *queue) {
    if (!queue) return NULL;

    return list_remove_head(queue->list);
}

// Get the size of the queue
size_t queue_size(const Queue *queue) {
    return queue ? list_size(queue->list) : 0;
}
