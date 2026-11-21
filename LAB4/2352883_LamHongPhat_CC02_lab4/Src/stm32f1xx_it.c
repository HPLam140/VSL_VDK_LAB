#include "main.h"
#include "stm32f1xx_it.h"

/* TIM2 handle is defined in main.c. */
extern TIM_HandleTypeDef htim2;

/**
  * @brief Non-maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief Bus fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief System service call.
  */
void SVC_Handler(void)
{
}

/**
  * @brief Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief Pendable service interrupt.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief HAL system tick.
  *
  * Maintains the HAL timebase used for timeouts.
  * The scheduler uses TIM2 separately.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/**
  * @brief TIM2 global interrupt.
  *
  * HAL_TIM_IRQHandler clears the timer interrupt flag and calls
  * HAL_TIM_PeriodElapsedCallback(), which is defined in main.c.
  */
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}
