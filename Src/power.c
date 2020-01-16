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

void SystemClock_Config(void);
void lp_stop_wfi();

void lp_stop_wfi(void) {
  HAL_SuspendTick();
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI); // Power Manage Between Iterations
  SystemClock_Config(); // Restore the clock settings after STOP2, exiting STOP2 does not restore them implicitly.
  HAL_ResumeTick();
}
