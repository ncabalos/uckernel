#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"

bool queue_register(struct queue * queue, void * data, uint16_t size,
                    uint16_t element_size)
{
    if(!queue) {
        return false;
    }

    if(!data) {
        return false;
    }

    if(!size) {
        return false;
    }

    if(!size) {
        return false;
    }

    queue->data = data;
    queue->size = size;
    queue->element_size = element_size;
    queue->count = 0;
    queue->in = 0;
    queue->out = 0;
    return true;
}
bool queue_enqueue(struct queue * queue, void * data)
{
    void * ptr;

    if(!queue) {
        return false;
    }

    if(!data) {
        return false;
    }

    /* check if queue is full */
    if(queue->count == queue->size) {
        return false;
    }

    /* put data into the queue */
    ptr = queue->data + (queue->in * queue->element_size);
    memcpy(ptr, data, queue->element_size);
    /* update the in */
    queue->in++;
    queue->in %= queue->size;
    /* update the count */
    queue->count++;
    return true;
}

bool queue_dequeue(struct queue * queue, void * data)
{
    void * ptr;

    if(!queue) {
        return false;
    }

    if(!data) {
        return false;
    }

    if(queue->count == 0) {
        return false;
    }

    ptr = queue->data + (queue->out * queue->element_size);
    memcpy(data, ptr, queue->element_size);
    /* update the out */
    queue->out++;
    queue->out %= queue->size;
    /* update the count */
    queue->count--;
    return true;
}

void * queue_peek_at(struct queue * queue, uint16_t index)
{
    void * ptr;

    if(!queue) {
        return NULL;
    }

    if(queue->count == 0) {
        return NULL;
    }

    ptr = queue->data + (index * queue->element_size);
    return ptr;
}
