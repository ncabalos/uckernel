#include <stdint.h>
#include <stdbool.h>

#include "queue.h"
#include "uckernel.h"

enum {
    TASK_EVENT_1,
    TASK_EVENT_2,
    TASK_EVENT_3,
    TASK_EVENT_4,
    TASK_EVENT_COUNT
};

static uckernel_event event_queue_task1[5];
static void task1(const uint16_t event, void * data)
{
    return;
}

static uckernel_event event_queue_task2[5];
static void task2(const uint16_t event, void * data)
{
    return;
}

int main(void)
{
    /* Initialize hardware */

    /* Register tasks */
    uckernel_task_register(task1, event_queue_task1, sizeof(event_queue_task1),
                           UCKERNEL_TASK_PRIORITY_MED);
    uckernel_task_register(task2, event_queue_task2, sizeof(event_queue_task2),
                           UCKERNEL_TASK_PRIORITY_MED);

    for(;;) {
        /* Run uckernel_event_loop */
        uckernel_event_loop();
    }

    return 0;
}
