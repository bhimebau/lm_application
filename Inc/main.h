/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
enum {COMMAND, SAMPLE};
extern uint32_t mode;
extern uint32_t mode_counter;
extern uint32_t mode_flag;
extern uint32_t power_lock_enable;
  
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* #define rtc_EVI_Pin GPIO_PIN_15 */
/* #define rtc_EVI_GPIO_Port GPIOC */
/* #define sm_237t_pwr_Pin GPIO_PIN_0 */
/* #define sm_237t_pwr_GPIO_Port GPIOA */
/* #define sm_237t_freq_Pin GPIO_PIN_1 */
/* #define sm_237t_freq_GPIO_Port GPIOA */
#define tsl237_pwr_Pin GPIO_PIN_4
#define tsl237_pwr_GPIO_Port GPIOA
#define tsl237_freq_Pin GPIO_PIN_0
#define tsl237_freq_GPIO_Port GPIOA
/* #define rtc_nINT_Pin GPIO_PIN_6 */
/* #define rtc_nINT_GPIO_Port GPIOA */
/* #define rtc_nINT_EXTI_IRQn EXTI9_5_IRQn */
/* #define rtc_clock_Pin GPIO_PIN_7 */
/* #define rtc_clock_GPIO_Port GPIOA */
/* #define led_out_Pin GPIO_PIN_0 */
/* #define led_out_GPIO_Port GPIOB */
#define sensor_int_Pin GPIO_PIN_8
#define sensor_int_GPIO_Port GPIOA
#define sensor_int_EXTI_IRQn EXTI9_5_IRQn
#define sensor_clock_Pin GPIO_PIN_9
#define sensor_clock_GPIO_Port GPIOA
#define sensor_data_Pin GPIO_PIN_10
#define sensor_data_GPIO_Port GPIOA
#define sensor_power_Pin GPIO_PIN_11
#define sensor_power_GPIO_Port GPIOA
#define rtc_data_Pin GPIO_PIN_4
#define rtc_data_GPIO_Port GPIOB

  // Adding these to 
#define batt_measure_Pin GPIO_PIN_6
#define batt_measure_GPIO_Port GPIOA
#define irange_0_Pin GPIO_PIN_4
#define irange_0_GPIO_Port GPIOB
#define irange_1_Pin GPIO_PIN_5
#define irange_1_GPIO_Port GPIOB
#define irange_2_Pin GPIO_PIN_6
#define irange_2_GPIO_Port GPIOB
#define irange_3_Pin GPIO_PIN_7
#define irange_3_GPIO_Port GPIOB
  
/* USER CODE BEGIN Private defines */

#define sensor_power_Pin GPIO_PIN_11
#define sensor_power_GPIO_Port GPIOA
  
#define SENSOR_POWER_ON  HAL_GPIO_WritePin(GPIOB, SW_MODE_Pin, GPIO_PIN_SET);
#define SENSOR_POWER_OFF  HAL_GPIO_WritePin(GPIOB, SW_MODE_Pin, GPIO_PIN_RESET);
#define TICK_FREQ_HZ 1000
  
void MX_DAC1_Init(void);


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
