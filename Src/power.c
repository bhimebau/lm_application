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

void SystemClock_Config(void);

void lp_stop_wfi(void) {
  SysTick->CTRL = 0;
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI); // Power Manage Between Iterations
  SystemClock_Config(); // Restore the clock settings after STOP2, exiting STOP2 does not restore them implicitly.
  SysTick_Config(SystemCoreClock/TICK_FREQ_HZ);   // Start systick rolling again
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
}
