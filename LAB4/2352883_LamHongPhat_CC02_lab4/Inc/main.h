#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* Function prototypes */
void Error_Handler(void);

/* Task LED outputs */
#define TASK1_Pin           GPIO_PIN_0
#define TASK1_GPIO_Port     GPIOA

#define TASK2_Pin           GPIO_PIN_1
#define TASK2_GPIO_Port     GPIOA

#define TASK3_Pin           GPIO_PIN_2
#define TASK3_GPIO_Port     GPIOA

#define TASK4_Pin           GPIO_PIN_3
#define TASK4_GPIO_Port     GPIOA

#define TASK5_Pin           GPIO_PIN_4
#define TASK5_GPIO_Port     GPIOA

/* Timer tick debug output */
#define TICK_Pin            GPIO_PIN_5
#define TICK_GPIO_Port      GPIOA

/*
 * Labels refer to the Virtual Terminal destinations:
 * PA9  = MCU USART1_TX -> terminal RXD
 * PA10 = MCU USART1_RX <- terminal TXD
 */
#define RXD_Pin             GPIO_PIN_9
#define RXD_GPIO_Port       GPIOA

#define TXD_Pin             GPIO_PIN_10
#define TXD_GPIO_Port       GPIOA

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
