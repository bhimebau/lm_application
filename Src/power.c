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
#include "power.h"

void SystemClock_Config(void);
void lp_stop_wfi();

void lp_stop_wfi(void) {
  HAL_SuspendTick();                           // Stop SysTick
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI); // Power Manage Between Iterations
  SystemClock_Config();                        // Restore the clock settings after STOP2, exiting STOP2 does not restore them implicitly.
  HAL_ResumeTick();                            // Start SysTick
}

// sensor_power
// Control the power switch that powers both the
// tsl237 light sensor and the current controlled
// led driver.
// Input:
// state: this has valid values of POWER_ON or POWER_OFF
int sensor_power(uint32_t state) {
  if (state == POWER_OFF) {
    HAL_GPIO_WritePin(GPIOA, tsl237_pwr_Pin, GPIO_PIN_RESET);
  }
  else if (state == POWER_ON) {
    HAL_GPIO_WritePin(GPIOA, tsl237_pwr_Pin, GPIO_PIN_SET);
  }
  else {
    return (-1);
  }
  return (0);
}
  
  
