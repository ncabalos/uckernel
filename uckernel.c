#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "version.h"

#include "queue.h"
#include "../uckernel_config.h"
#include "uckernel.h"

struct delayed_event {
    bool in_use;
    uckernel_task func;
    uint16_t delay_ms;
    uckernel_event event;
};

struct task_control_block {
    char task_name[UCKERNEL_TASKNAME_SIZE];
    uckernel_task func;
    struct queue event_q;
    bool pending;
};

struct uckernel_info_block {
    char version_string[10];
    char sha_string[8];
    uint16_t task_count;
    uint16_t delayed_task_count;
    uint16_t post_event_count;
    uint16_t post_event_failure;
};

static struct uckernel_info_block uckernel_info_block;

static struct task_control_block
        tasks[UCKERNEL_TASK_PRIORITY_COUNT * UCKERNEL_TASK_QUEUE_SIZE];

static uint16_t delayed_event_count = 0;
static struct delayed_event
        delayed_event_list[UCKERNEL_DELAYED_EVENT_LIST_SIZE];
static uckernel_event event_queue_update_delay_task[1];
static void uckernel_update_delay_task(const uint16_t event, void * data)
{
    struct delayed_event * d_event;
    uint16_t i;
    bool result;

    if(delayed_event_count == 0) {
        return;
    }

    for (i = 0; i < UCKERNEL_DELAYED_EVENT_LIST_SIZE; i++) {
        d_event = &delayed_event_list[i];

        if (d_event->in_use == true) {
            if(d_event->delay_ms > 0) {
                d_event->delay_ms--;
            }

            if (d_event->delay_ms == 0) {
                /* Put in event loop */
                result = uckernel_post_event(d_event->func, d_event->event.event,
                                             d_event->event.data, 0);

                if(result == true) {
                    d_event->in_use = false;
                    d_event->delay_ms = 0;
                    d_event->event.event = 0;
                    d_event->event.data = NULL;
                    delayed_event_count--;
                    uckernel_info_block.delayed_task_count--;
                }
            }
        }
    }

    return;
}

static void reset_uckernel_info_block(void)
{
    strcpy(uckernel_info_block.version_string, VERSION_NUM);
    strcpy(uckernel_info_block.sha_string, VERSION_SHA);
    uckernel_info_block.task_count = 0;
    uckernel_info_block.delayed_task_count = 0;
    uckernel_info_block.post_event_count = 0;
    uckernel_info_block.post_event_failure = 0;
}

bool uckernel_init(void)
{
    bool result;
    reset_uckernel_info_block();
    result = uckernel_task_register("uckernel_tick", uckernel_update_delay_task,
                                    event_queue_update_delay_task,
                                    sizeof(event_queue_update_delay_task),
                                    UCKERNEL_TASK_PRIORITY_HIGH);
    return result;
}

bool uckernel_task_register(char * task_name, uckernel_task func,
                            uckernel_event * event_queue,
                            uint16_t event_queue_size, uint16_t priority)
{
    uint16_t i;
    bool result;
    struct task_control_block * tcb_ptr;

    if(priority >= UCKERNEL_TASK_PRIORITY_COUNT) {
        return false;
    }

    for(i = 0; i < UCKERNEL_TASK_QUEUE_SIZE; i++) {
        tcb_ptr = (struct task_control_block *) (tasks + (priority *
                  UCKERNEL_TASK_QUEUE_SIZE + i));

        if(tcb_ptr->func == NULL) {
            tcb_ptr->func = func;
            strcpy(tcb_ptr->task_name, task_name);
            result = queue_register(&tcb_ptr->event_q, (void *) event_queue,
                                    event_queue_size, sizeof(uckernel_event));

            if(result == true) {
                uckernel_info_block.task_count++;
            }

            return result;
        }
    }

    return false;
}

void uckernel_event_loop(void)
{
    struct task_control_block * tcb_ptr;
    uckernel_event pending_event;
    uint16_t i;

    for(i = 0;
        i < (uint16_t) UCKERNEL_TASK_PRIORITY_COUNT * UCKERNEL_TASK_QUEUE_SIZE;
        i++) {
        tcb_ptr = (struct task_control_block *)(tasks + i);

        if( tcb_ptr != NULL && tcb_ptr->pending == true ) {
            if(queue_dequeue(&tcb_ptr->event_q, &pending_event) == false ) {
                return;
            }

            tcb_ptr->func(pending_event.event, pending_event.data);

            if(tcb_ptr->event_q.count == 0) {
                tcb_ptr->pending = false;
            }

            return;
        }
    }
}

static bool tcb_post_delayed_event(struct task_control_block * tcb,
                                   uckernel_event * event, uint16_t delay_ms)
{
    struct delayed_event * d_event;
    uint16_t i;

    if(tcb == NULL) {
        return false;
    }

    if(event == NULL) {
        return false;
    }

    for(i = 0; i < UCKERNEL_DELAYED_EVENT_LIST_SIZE; i++) {
        d_event = &delayed_event_list[i];

        if(d_event->in_use == false) {
            d_event->func = tcb->func;
            d_event->delay_ms = delay_ms;
            d_event->event.data = event->data;
            d_event->event.event = event->event;
            d_event->in_use = true;
            delayed_event_count++;
            return true;
        }
    }

    return false;
}

static bool tcb_post_event(struct task_control_block * tcb, uint16_t event,
                           void * data, uint16_t delay_ms)
{
    bool result;
    uckernel_event tcb_event;

    if(tcb == NULL) {
        return false;
    }

    if (delay_ms == 0) {
        tcb_event.event = event;
        tcb_event.data = data;
        uckernel_info_block.post_event_count++;
        result = queue_enqueue(&tcb->event_q, &tcb_event);

        if(result == false) {
            uckernel_info_block.post_event_failure++;
        }
    } else {
        result = tcb_post_delayed_event(tcb, &tcb_event, delay_ms);

        if(result == true) {
            uckernel_info_block.delayed_task_count++;
        }
    }

    return result;
}

static struct task_control_block * get_tcb(uckernel_task func)
{
    struct task_control_block * tcb_ptr;
    uint16_t i;
    //tcb_ptr = NULL;

    if(func == NULL) {
        return NULL;
    }

    for(i = 0;
        i < (uint16_t)UCKERNEL_TASK_PRIORITY_COUNT * UCKERNEL_TASK_QUEUE_SIZE;
        i++) {
        tcb_ptr = (struct task_control_block *)(tasks + i);

        if(tcb_ptr->func == func) {
            return tcb_ptr;
        }
    }

    return NULL;
}

bool uckernel_post_event(const uckernel_task func, uint16_t event,
                         void * data, uint16_t delay_ms)
{
    struct task_control_block * tcb_ptr = NULL;
    bool result;
    result = false;
    tcb_ptr = get_tcb(func);

    if(tcb_ptr != NULL) {
        result = tcb_post_event(tcb_ptr, event, data, delay_ms);

        if(result) {
            tcb_ptr->pending = true;
        }
    }

    return result;
}

void uckernel_tick_handler(void)
{
    uckernel_post_event(uckernel_update_delay_task, NULL, NULL, 0);
}
