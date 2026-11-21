#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define SCH_TICK_MS           10U
#define SCH_MAX_TASKS         16U
#define SCH_INVALID_TASK_ID   0U
#define SCH_MAX_INTERVAL      0x7FFFFFFFU
/* Use whole multiples of 10 ms; arguments to SCH_Add_Task are TICKS. */
#define SCH_MS_TO_TICKS(ms)   ((uint32_t)(ms) / SCH_TICK_MS)

typedef enum {
    SCH_OK = 0,
    SCH_ERROR_FULL,
    SCH_ERROR_BAD_CALLBACK,
    SCH_ERROR_BAD_INTERVAL,
    SCH_ERROR_BAD_ID
} SCH_Error;

extern volatile SCH_Error Error_code_G;

void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY,
                      uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID); /* 0 = success, 1 = error */
uint32_t get_time(void);                /* Elapsed milliseconds */

#endif
