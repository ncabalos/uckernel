#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "queue.h"
#include "uckernel.h"

#define TASK_QUEUE_SIZE 5

struct task_control_block {
    uckernel_task func;
    struct queue event_queue;
    bool pending;
};

static struct task_control_block
        tasks[UCKERNEL_TASK_PRIORITY_COUNT * TASK_QUEUE_SIZE];

bool uckernel_task_register(uckernel_task func, uckernel_event * event_queue,
                            uint16_t event_queue_size, uint16_t priority)
{
    uint16_t i;
    struct task_control_block * tcb_ptr;

    if(priority >= UCKERNEL_TASK_PRIORITY_COUNT) {
        return false;
    }

    for(i = 0; i < TASK_QUEUE_SIZE; i++) {
        tcb_ptr = (struct task_control_block *) (tasks + (priority * TASK_QUEUE_SIZE +
                  i));

        if(tcb_ptr->func == NULL) {
            tcb_ptr->func = func;
            return queue_register(&tcb_ptr->event_queue, (void *) event_queue,
                                  event_queue_size, sizeof(uckernel_event));
        }
    }

    return false;
}

void uckernel_event_loop(void)
{
    struct task_control_block * tcb_ptr;
    uckernel_event pending_event;
    uint16_t i;

    for(i = 0; i < (uint16_t) UCKERNEL_TASK_PRIORITY_COUNT * TASK_QUEUE_SIZE;
        i++) {
        tcb_ptr = (struct task_control_block *)(tasks + i);

        if( tcb_ptr != NULL && tcb_ptr->pending == true ) {
            if(queue_dequeue(&tcb_ptr->event_queue, &pending_event) == false ) {
                return;
            }

            tcb_ptr->func(pending_event.event, pending_event.data);

            if(tcb_ptr->event_queue.count == 0) {
                tcb_ptr->pending = false;
            }

            return;
        }
    }
}

static bool tcb_post_event(struct task_control_block * tcb, uint16_t event,
                           void * data, uint16_t delay_ms)
{
    uckernel_event tcb_event;
    tcb_event.event = event;
    tcb_event.data = data;
    tcb_event.delay_ms = delay_ms;
    return queue_enqueue(&tcb->event_queue, &tcb_event);
}

static struct task_control_block * get_tcb(const uckernel_task func)
{
    struct task_control_block * tcb_ptr;
    uint16_t i;
    tcb_ptr = NULL;

    for(i = 0; i < (uint16_t)UCKERNEL_TASK_PRIORITY_COUNT * TASK_QUEUE_SIZE;
        i++) {
        tcb_ptr = (struct task_control_block *)(tasks + i);

        if(tcb_ptr->func == func) {
            return tcb_ptr;
        }
    }

    return tcb_ptr;
}

bool uckernel_post_event(const uckernel_task func, uint16_t event, void * data)
{
    struct task_control_block * tcb_ptr = NULL;
    bool result;
    result = false;
    tcb_ptr = get_tcb(func);

    if(tcb_ptr != NULL) {
        result = tcb_post_event(tcb_ptr, event, data, 0);

        if(result) {
            tcb_ptr->pending = true;
        }
    }

    return result;
}
