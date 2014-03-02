#ifndef UCKERNEL_H
#define	UCKERNEL_H

enum {
    UCKERNEL_TASK_PRIORITY_HIGH,
    UCKERNEL_TASK_PRIORITY_MED,
    UCKERNEL_TASK_PRIORITY_LOW,
    UCKERNEL_TASK_PRIORITY_COUNT
};

typedef void (* uckernel_task)(const uint16_t event, void * data);

typedef struct uckernel_event_s {
    uint16_t event;
    void * data;
    uint16_t delay_ms;
} uckernel_event;

bool uckernel_task_register(uckernel_task func, uckernel_event * event_queue,
                            uint16_t event_queue_size, uint16_t priority);
void uckernel_event_loop(void);
bool uckernel_post_event(const uckernel_task func, uint16_t event, void * data,
                         uint16_t delay_ms);

#endif	/* UCKERNEL_H */

