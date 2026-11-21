#include "main.h"

/**
  * @brief Global MCU Support Package initialization.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Disable JTAG while keeping Serial Wire Debug enabled. */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
}

/**
  * @brief TIM2 hardware initialization.
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
  if (htim_base->Instance == TIM2)
  {
    /* Enable TIM2 peripheral clock. */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* Enable TIM2 interrupt in the NVIC. */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1U, 0U);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
}

/**
  * @brief TIM2 hardware de-initialization.
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim_base)
{
  if (htim_base->Instance == TIM2)
  {
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
    __HAL_RCC_TIM2_CLK_DISABLE();
  }
}

/**
  * @brief USART1 hardware initialization.
  *
  * PA9  = USART1_TX -> Virtual Terminal RXD
  * PA10 = USART1_RX <- Virtual Terminal TXD
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (huart->Instance == USART1)
  {
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*
     * Disable USART1 remapping.
     * This selects PA9/PA10 instead of PB6/PB7.
     */
    __HAL_AFIO_REMAP_USART1_DISABLE();

    /* PA9: USART1 TX, alternate-function push-pull. */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA10: USART1 RX, input. */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

/**
  * @brief USART1 hardware de-initialization.
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
  }
}
