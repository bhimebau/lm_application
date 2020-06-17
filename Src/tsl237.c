/* tls25911.c
 * Description: Functions used to communicate with the TLS25911 light sensor. 
 *
 * Author: Bryce Himebaugh  * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "tsl237.h"
#include "flash.h"
#include "power.h"
#include "cal.h"

//#define NUM_SAMPLES 1000
#define NUM_SAMPLES 8
#define MAX_ARGS 3

TIM_HandleTypeDef htim2;

uint32_t tsl237_done = 0;
uint32_t tsl237t_done = 0;

extern flash_status_t fs;

// Buffer for the TSL samples
// Buffer is two larger than needed
// because the sensor sends two mis-sized pulses
// after power on. 
uint32_t buf[NUM_SAMPLES+2] = {0}; // Buffer values initialized to 0


void tsl237_command(char *arguments) {
  char * argv[MAX_ARGS];
  char *p = arguments;
  int i;
  int value;
  uint32_t raw;
  
  if (!arguments) {
    printf("NOK1\n\r"); //RM
    return;
  }
  else {
    // Zero the arguments array 
    for (i=0;i<MAX_ARGS;i++) {
      argv[i] = (char *) 0;
    }
    argv[0] = p;
    i = 1;
    // locate the other arguments
    while ((*p) && (i < MAX_ARGS)){
      if (*p==',') {
        *p=0;
        argv[i++] = p+1;
      }
      p++;
    }
  }
  if (!strcmp(argv[0],"raw")) {
    sensor_power(POWER_ON);
    HAL_TIM_Base_Init(&htim2);
    HAL_TIM_IC_Init(&htim2);
    HAL_Delay(3);
    printf("%d\n\r",(int) tsl237_readsensor());
    HAL_TIM_Base_DeInit(&htim2);
    HAL_TIM_IC_DeInit(&htim2);
    sensor_power(POWER_OFF);
    printf("OK\n\r");

  }
  else if (!strcmp(argv[0],"mag")) {
    sensor_power(POWER_ON);
    HAL_TIM_Base_Init(&htim2);
    HAL_TIM_IC_Init(&htim2);
    HAL_Delay(3);
    raw = tsl237_readsensor();
    value = cal_lookup(raw);
    if ((value == 1) || (value == -1)) {
        printf("%f\n\r",(float) raw);
    }
    else {
      printf("%d.%02d\n\r",value/100,value%100);
    }
    HAL_TIM_Base_DeInit(&htim2);
    HAL_TIM_IC_DeInit(&htim2);
    sensor_power(POWER_OFF);
    printf("OK\n\r");
  }
  else {
    printf("NOK2\n\r"); //RM
  }    
}

uint32_t tsl237_readsensor() {
  char print_buffer[100];
  long long sum = 0;
  //float average_period;
  uint32_t average_period;
  int i;
  // sensor_power(POWER_ON);
  //tsl237_vdd_on();  // Power on the sensor
  HAL_Delay(1000); // Wait a second to allow the sensor to stabilize
  //  MX_DMA_Init();
  //  HAL_TIM_Base_Init(&htim2);
  uint32_t start_time = HAL_GetTick();
  tsl237_done = 0;
  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*) buf, NUM_SAMPLES+2);
  
  while (1) { 
    if (tsl237_done != 0) {
      for (i=3;i<NUM_SAMPLES+2;i++) {
        if (buf[i] >= buf[i-1]) {
          sum += (long long) (buf[i] - buf[i-1]);
          printf("GTE %d\n\r",(int) (buf[i]-buf[i-1]));
        }
        else {
          sum += (long long) ((htim2.Instance->ARR - buf[i-1]) + buf[i]);
          printf("ARR = %u\n\r",(unsigned int) htim2.Instance->ARR);
          printf("buf[i-1] = %u\n\r",(unsigned int) buf[i-1]);
          printf("buf[i] = %u\n\r",(unsigned int) buf[i]);
          printf("LT %d\n\r",(int) (((uint32_t) htim2.Instance->ARR) - buf[i-1] + buf[i]));
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
          sprintf(print_buffer,"Note: Short Read of %d samples after %d seconds",i-1,MAX_SAMPLE_TIME/1000);
          write_log_data(&fs,print_buffer);
          break;
        }
      }
      average_period = sum/(i-1);  // Compute the average given the shortened number of edges.
      //      printf("avg %d %d\n\r",(int)average_period,i-1);
      break;
    }
  }
  // Power Savings
  // sensor_power(POWER_OFF);
  /* tsl237_vdd_off(); // Turn off the sensor to save power  */
  //  __HAL_RCC_DMA1_CLK_DISABLE(); // Kick off the clock to the DMA controller
  //   HAL_TIM_Base_DeInit(&htim2);  // Uninitialize timer 2 to save power
  //  return ((float) HAL_RCC_GetHCLKFreq() / average_period);
  printf("Average Period = %d\n\r",(int) average_period);
  return(average_period);
}



