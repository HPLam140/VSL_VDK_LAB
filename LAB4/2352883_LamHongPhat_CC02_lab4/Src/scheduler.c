#include "scheduler.h"
#include <stddef.h>

typedef struct Task {
    void (*callback)(void);
    uint32_t deadline;
    uint32_t period;
    uint32_t id;
    struct Task *next;
} Task;

static Task pool[SCH_MAX_TASKS];
static Task *head;
static volatile uint32_t ticks;
static uint32_t next_id = 1U;
volatile SCH_Error Error_code_G;

/* Modular comparison: supported separations are at most INT32_MAX ticks. */
static uint8_t earlier(uint32_t a, uint32_t b)
{
    return (uint8_t)((a != b) && ((b - a) < 0x80000000U));
}

/* Main-loop context only. Equal deadlines retain FIFO insertion order. */
static void insert(Task *task)
{
    Task **link = &head;
    while ((*link != NULL) && !earlier(task->deadline, (*link)->deadline)) {
        link = &(*link)->next;
    }
    task->next = *link;
    *link = task;
}

void SCH_Init(void)
{
    uint32_t i;
    /* Call at startup, with the scheduler timer stopped. */
    head = NULL;
    ticks = 0U;
    Error_code_G = SCH_OK;
    for (i = 0U; i < SCH_MAX_TASKS; ++i) {
        pool[i].callback = NULL;
        pool[i].deadline = 0U;
        pool[i].period = 0U;
        pool[i].id = SCH_INVALID_TASK_ID;
        pool[i].next = NULL;
    }
    /* Do not reuse IDs across reinitialization. */
}

void SCH_Update(void)
{
    /* TIM2 ISR: O(1), no searches, callback execution, or UART output. */
    ++ticks;
}

uint32_t get_time(void)
{
    /* Aligned 32-bit read is atomic on Cortex-M3. Wraps after ~49.7 days. */
    return ticks * SCH_TICK_MS;
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY,
                      uint32_t PERIOD)
{
    uint32_t i;
    Task *task = NULL;

    if (pFunction == NULL) {
        Error_code_G = SCH_ERROR_BAD_CALLBACK;
        return SCH_INVALID_TASK_ID;
    }
    if ((DELAY > SCH_MAX_INTERVAL) || (PERIOD > SCH_MAX_INTERVAL)) {
        Error_code_G = SCH_ERROR_BAD_INTERVAL;
        return SCH_INVALID_TASK_ID;
    }
    for (i = 0U; i < SCH_MAX_TASKS; ++i) {
        if (pool[i].callback == NULL) {
            task = &pool[i];
            break;
        }
    }
    if (task == NULL) {
        Error_code_G = SCH_ERROR_FULL;
        return SCH_INVALID_TASK_ID;
    }

    task->callback = pFunction;
    task->deadline = ticks + DELAY;
    task->period = PERIOD;
    task->id = next_id++;
    if (next_id == SCH_INVALID_TASK_ID) {
        next_id = 1U;
    }
    insert(task);
    return task->id;
}

uint8_t SCH_Delete_Task(uint32_t taskID)
{
    Task **link = &head;
    while (*link != NULL) {
        Task *task = *link;
        if (task->id == taskID) {
            *link = task->next;
            task->callback = NULL;
            task->id = SCH_INVALID_TASK_ID;
            task->next = NULL;
            return 0U;
        }
        link = &task->next;
    }
    Error_code_G = SCH_ERROR_BAD_ID;
    return 1U;
}

void SCH_Dispatch_Tasks(void)
{
    Task *task = head;
    void (*callback)(void);
    uint32_t now = ticks;

    /* One callback per call; main calls this continuously. */
    if ((task == NULL) || earlier(now, task->deadline)) {
        return;
    }

    head = task->next;
    callback = task->callback;
    if (task->period != 0U) {
        /* Anchor to the previous deadline, not the completion time.
         * Overdue releases are caught up in deadline order. Reinsert before
         * calling so a callback can delete itself or register another task. */
        task->deadline += task->period;
        insert(task);
    } else {
        /* Free one-shot slot before execution; its old ID is now invalid. */
        task->callback = NULL;
        task->id = SCH_INVALID_TASK_ID;
        task->next = NULL;
    }
    callback();
}
