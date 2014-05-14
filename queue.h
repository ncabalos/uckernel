#ifndef QUEUE_H
#define	QUEUE_H

#include <stdbool.h>

struct queue {
    void * data;
    uint16_t size;
    uint16_t count;
    uint16_t element_size;
    uint16_t in;
    uint16_t out;
};

bool queue_register(struct queue * queue, void * data, uint16_t size,
                    uint16_t element_size);
bool queue_enqueue(struct queue * queue, void * data);
bool queue_dequeue(struct queue * queue, void * data);
void * queue_peek_at(struct queue * queue, uint16_t index);

#endif	/* STATIC_QUEUE_H */

