/* power.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 06.24.2019
 * Copyright (C) 2019
 *
 */
#include "main.h"
#include <stm32l4xx_ll_lpuart.h>
#include <stdio.h>

extern UART_HandleTypeDef hlpuart1;


void SystemClock_Config(void);
void lp_stop_wfi();

void stop_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    printf("Entering STOP2\n\r");
    lp_stop_wfi();
    printf("Returning from STOP2\n\r");
  }
}

void i2c_pin_configure(void) {
  // Configure the I2C1 and I2C3 pins as analog input with a pullup. 
  // PA7 rtc_clock
  // PB4 rtc_data
  // PA9 sensor_clock
  // PA10 sensor_data
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void lp_stop_wfi(void) {
  // i2c_pin_configure();
  //   SysTick->CTRL = 0;
  HAL_SuspendTick();
  /* __HAL_RCC_GPIOC_CLK_DISABLE(); */
  /* __HAL_RCC_GPIOA_CLK_DISABLE(); */
  /* __HAL_RCC_GPIOB_CLK_DISABLE(); */
  /* __HAL_RCC_GPIOH_CLK_DISABLE(); */
  /* if (LL_LPUART_IsEnabledInStopMode(LPUART1)) { */
  /*   printf("\n\rLPUART Enabled in STOP\n\r"); */
  /* } */
  /* else { */
  /*   printf("\n\rLPUART Disabled in STOP\n\r"); */
  /* } */
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI); // Power Manage Between Iterations
  SystemClock_Config(); // Restore the clock settings after STOP2, exiting STOP2 does not restore them implicitly.
  //  SysTick_Config(SystemCoreClock/TICK_FREQ_HZ);   // Start systick rolling again
  HAL_ResumeTick();
  /* __HAL_RCC_GPIOC_CLK_ENABLE(); */
  /* __HAL_RCC_GPIOB_CLK_ENABLE(); */
  /* __HAL_RCC_GPIOA_CLK_ENABLE(); */
}
