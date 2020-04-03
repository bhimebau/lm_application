/* tls25911.c
 * Description: Functions used to communicate with the TLS25911 light sensor. 
 *
 * Author: Bryce Himebaugh  * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include "tsl237.h"
#include "flash.h"

#define NUM_SAMPLES 1000

TIM_HandleTypeDef htim2;

uint32_t tsl237_done = 0;
uint32_t tsl237t_done = 0;

extern flash_status_t fs;

void tsl237_vdd_on(void) {
  /* GPIO_InitTypeDef GPIO_InitStruct = {0}; */
  /* GPIO_InitStruct.Pin = tsl237_pwr_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; */
  /* HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); */
  HAL_GPIO_WritePin(GPIOA, tsl237_pwr_Pin, GPIO_PIN_SET);
}
  
void tsl237_vdd_off(void) {
  /* GPIO_InitTypeDef GPIO_InitStruct = {0}; */
  /* GPIO_InitStruct.Pin = tsl237_pwr_Pin; */
  /* //   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; */
  /* //   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* GPIO_InitStruct.Pull = GPIO_PULLDOWN; */
  /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; */
  /* HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); */
  HAL_GPIO_WritePin(GPIOA, tsl237_pwr_Pin, GPIO_PIN_RESET);
}

void tsl237_command(char *arguments) {
  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    //  printf("%.3f\n\r",(double) tsl237_readsensor());
    printf("%dn\r",(int) tsl237_readsensor());
    printf("OK\n\r");
  }
}

uint32_t tsl237_readsensor() {
  char num_buf[8];
  long long sum = 0;
  //float average_period;
  uint32_t average_period;
  int i;
  uint32_t buf[NUM_SAMPLES] = {0}; // Buffer values initialized to 0 
  tsl237_vdd_on();  // Power on the sensor
  HAL_Delay(100); // Wait 100mS to allow the boost converter to stabibilize
  //  MX_DMA_Init();
  //  HAL_TIM_Base_Init(&htim2);
  uint32_t start_time = HAL_GetTick();
  tsl237_done = 0;
  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*) buf, NUM_SAMPLES);
  
  while (1) {
    if (tsl237_done != 0) {
      for (i=1;i<NUM_SAMPLES;i++) {
        if (buf[i] >= buf[i-1]) {
          sum += (long long) (buf[i] - buf[i-1]);
        }
        else {
          sum += (long long) ((htim2.Instance->ARR - buf[i-1]) + buf[i]);
        }
      }
      //    average_period = (float) sum/(NUM_SAMPLES-1);  // Compute the average
      average_period = sum/(NUM_SAMPLES-1);  // Compute the average
      break;
    }
    else if ((HAL_GetTick()-start_time) > MAX_SAMPLE_TIME) {
      //      printf("time = %d\n\r",(int) (HAL_GetTick()-start_time));
      // If the time has exceeded MAX_SAMPLE_TIME, abort the sampling
      HAL_TIM_IC_Stop_DMA(&htim2, TIM_CHANNEL_1);
      for (i=1;i<NUM_SAMPLES;i++) {
        if (buf[i]) {
          if (buf[i] >= buf[i-1]) {
            sum += (long long) (buf[i] - buf[i-1]);
          }
          else {
            sum += (long long) ((htim2.Instance->ARR - buf[i-1]) + buf[i]);
          }
          //          printf("%d %d %d %d\n\r", i, (int) buf[i-1], (int) buf[i], (int) (buf[i]-buf[i-1]));
        }
        else {
          sprintf(num_buf,"sr-%d",i-1);
          write_log_data(&fs,num_buf);
          break;
        }
      }
      average_period = sum/(i-1);  // Compute the average given the shortened number of edges.
      //      printf("avg %d %d\n\r",(int)average_period,i-1);
      break;
    }
  }
  // Power Savings
  tsl237_vdd_off(); // Turn off the sensor to save power 
  //  __HAL_RCC_DMA1_CLK_DISABLE(); // Kick off the clock to the DMA controller
  //   HAL_TIM_Base_DeInit(&htim2);  // Uninitialize timer 2 to save power
  //  return ((float) HAL_RCC_GetHCLKFreq() / average_period);
  return(average_period);
}



