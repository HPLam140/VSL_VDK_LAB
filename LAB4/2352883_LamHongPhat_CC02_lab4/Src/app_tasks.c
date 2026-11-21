#include "main.h"
#include "scheduler.h"
#include "app_tasks.h"
#include <stddef.h>
#ifndef APP_PRINT_10MS
#define APP_PRINT_10MS 0U
#endif

/* Works with CubeIDE's single-main.c or per-peripheral file generation. */
extern UART_HandleTypeDef huart1;
static uint32_t victim_id;

static void uart_text(const char *text)
{
    uint16_t length = 0U;
    while (text[length] != '\0') {
        ++length;
    }
    if (HAL_UART_Transmit(&huart1, (uint8_t *)text, length, 100U) != HAL_OK) {
        Error_Handler();
    }
}
static uint32_t append_number(char *line,
                              uint32_t pos,
                              uint32_t value)
{
    char digits[10];
    uint32_t count = 0U;

    do {
        digits[count++] = (char)('0' + value % 10U);
        value /= 10U;
    } while (value != 0U);

    while (count != 0U) {
        line[pos++] = digits[--count];
    }

    return pos;
}

static uint32_t append_text(char *line,
                            uint32_t pos,
                            const char *text)
{
    while (*text != '\0') {
        line[pos++] = *text++;
    }

    return pos;
}

static void log_time(const char *name)
{
    char line[40];
    uint32_t pos = 0U;
    uint32_t time_ms = get_time();

    pos = append_text(line, pos, name);
    pos = append_text(line, pos, " ");
    pos = append_number(line, pos, time_ms);
    pos = append_text(line, pos, "\r\n");

    line[pos] = '\0';

    uart_text(line);
}

static void toggle_and_log(const char *name,
                           GPIO_TypeDef *port,
                           uint16_t pin)
{
    char line[64];
    uint32_t pos = 0U;
    uint32_t time_ms;
    GPIO_PinState state;

    /* Change the LED and immediately capture its transition time. */
    HAL_GPIO_TogglePin(port, pin);
    time_ms = get_time();
    state = HAL_GPIO_ReadPin(port, pin);

    /* Assumes GPIO HIGH = LED ON. */
    pos = append_text(line, pos, name);
    pos = append_text(
        line, pos,
        state == GPIO_PIN_SET ? "=ON" : "=OFF"
    );

    pos = append_text(line, pos, " | tick=");
    pos = append_number(line, pos, time_ms / SCH_TICK_MS);

    pos = append_text(line, pos, " | time=");
    pos = append_number(line, pos, time_ms);

    pos = append_text(line, pos, " ms\r\n");
    line[pos] = '\0';

    uart_text(line);
}

void task1(void)
{
    toggle_and_log("L1", TASK1_GPIO_Port, TASK1_Pin);
}

void task2(void)
{
    toggle_and_log("L2", TASK2_GPIO_Port, TASK2_Pin);
}

void task3(void)
{
    toggle_and_log("L3", TASK3_GPIO_Port, TASK3_Pin);
}

void task4(void)
{
    toggle_and_log("L4", TASK4_GPIO_Port, TASK4_Pin);
}

void task5(void)
{
    toggle_and_log("L5", TASK5_GPIO_Port, TASK5_Pin);
}

static void stamp10(void)
{
#if APP_PRINT_10MS
    log_time("T");
#endif
}
static void once_a(void) { log_time("A"); }
static void once_b(void) { log_time("B"); }
static void once_c(void) { log_time("C"); }
static void same_d(void) { log_time("D"); }
static void same_e(void) { log_time("E"); }
static void victim(void) { log_time("V"); }
static void nothing(void) { }

static void delete_victim(void)
{
    if (SCH_Delete_Task(victim_id) != 0U) {
        Error_Handler();
    }
    log_time("DEL V");
}

static void add(void (*callback)(void), uint32_t delay_ms, uint32_t period_ms)
{
    if (SCH_Add_Task(callback, SCH_MS_TO_TICKS(delay_ms),
                     SCH_MS_TO_TICKS(period_ms)) == SCH_INVALID_TASK_ID) {
        Error_Handler();
    }
}

/* Timer is still STOPPED. These checks do not advance simulated time. */
static void interface_checks(void)
{
    uint32_t i;
    uint32_t ids[SCH_MAX_TASKS];
    SCH_Init();
    if ((SCH_Add_Task(NULL, 1U, 0U) != SCH_INVALID_TASK_ID) ||
        (Error_code_G != SCH_ERROR_BAD_CALLBACK)) {
        Error_Handler();
    }
    if ((SCH_Delete_Task(SCH_INVALID_TASK_ID) == 0U) ||
        (Error_code_G != SCH_ERROR_BAD_ID)) {
        Error_Handler();
    }
    if ((SCH_Add_Task(nothing, 0x80000000U, 0U) != SCH_INVALID_TASK_ID) ||
        (Error_code_G != SCH_ERROR_BAD_INTERVAL)) {
        Error_Handler();
    }
    for (i = 0U; i < SCH_MAX_TASKS; ++i) {
        ids[i] = SCH_Add_Task(nothing, 1000U, 0U);
        if (ids[i] == SCH_INVALID_TASK_ID) {
            Error_Handler();
        }
    }
    if ((SCH_Add_Task(nothing, 1U, 0U) != SCH_INVALID_TASK_ID) ||
        (Error_code_G != SCH_ERROR_FULL)) {
        Error_Handler();
    }
    for (i = 0U; i < SCH_MAX_TASKS; ++i) {
        if (SCH_Delete_Task(ids[i]) != 0U) {
            Error_Handler();
        }
    }
    uart_text("CHECKS PASS: null, invalid ID, range, full, delete\r\n");
}

void APP_Init(void)
{
    uart_text("LAB4: 10ms tick; all timestamps in ms\r\n");
    interface_checks();
    SCH_Init(); /* Clears startup test errors and resets time to zero. */

    add(task1, 500U, 500U);
    add(task2, 1000U, 1000U);
    add(task3, 1500U, 1500U);
    add(task4, 2000U, 2000U);
    add(task5, 2500U, 2500U);
    add(stamp10, 10U, 10U);

    /* Registered A,C,B; expected execution order B,C,A. */
    add(once_a, 300U, 0U);
    add(once_c, 200U, 0U);
    add(once_b, 100U, 0U);
    add(same_d, 400U, 0U);
    add(same_e, 400U, 0U);

    victim_id = SCH_Add_Task(victim, SCH_MS_TO_TICKS(700U),
                             SCH_MS_TO_TICKS(700U));
    if (victim_id == SCH_INVALID_TASK_ID) {
        Error_Handler();
    }
    add(delete_victim, 2300U, 0U);
    uart_text("READY: L1-L5, T, B/C/A, D/E, V deletion\r\n");
}
