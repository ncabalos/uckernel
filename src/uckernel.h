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
} uckernel_event;

/**
 * This initializes the uckernel and sets up the required resources. The system
 * tick timer must be initialized before this function is called.
 * 
 * @return true = success; false = failed
 */
bool uckernel_init(void);
/**
 * This should be called every iteration of the main loop to allow the uckernel
 * to process events.
 */
void uckernel_event_loop(void);
bool uckernel_post_event(const uckernel_task func, uint16_t event, void * data,
                         uint16_t delay_ms);
bool uckernel_task_register(char * task_name, uckernel_task func,
                            uckernel_event * event_queue,
                            uint16_t event_queue_size, uint16_t priority);
/**
 * This should be called every tick event to process posted delayed events.
 */
void uckernel_tick_handler(void);

#endif	/* UCKERNEL_H */

