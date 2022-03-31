
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "retarget.h"
#include "flash.h"
#include "rtc.h"
#include "temperature.h"
#include "battery.h"
#include "command.h"
#include <stm32l4xx_ll_lpuart.h>
#include <stm32l4xx_ll_bus.h>
#include "queue.h"
#include "power.h"
#include "tsl237.h"
#include "led.h"
#include "sample.h"
#include "cal.h"
#include "option_byte.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* #define TICK_FREQ_HZ 100 */
#define LIGHT_SENSOR_ADDRESS (0x29<<1)
#define DELAY_1S 100
#define WU_UART 0x01
#define WU_RTC 0x02
#define COMMAND_TIMEOUT 1000
#define SAMPLE_INTERVAL_MINUTES 10
#define EVENING_START_HOUR 17 // Real-Time Clock Value in BCD 
#define MORNING_END_HOUR 8   // Real-Time Clock Value in BCD 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DAC_HandleTypeDef hdac1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef hlpuart1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC1_Init(void);
void MX_DAC1_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
extern uint8_t led_state;
RTC_TimeTypeDef current_time;
RTC_DateTypeDef current_date;
uint32_t rtc_counter = 0;
flash_status_t fs;
uint32_t collect_data_flag = 0;
uint32_t sample_interval = 0;
/* char command[MAX_COMMAND_LEN]; */
/* int lpuart_rx_flag = 0; */
queue_t rx_queue;
uint32_t mode = COMMAND;
uint32_t mode_counter = 0;
uint32_t mode_flag = 0;

uint32_t captures[2];
volatile uint8_t captureDone = 0;
float frequency = 0;
uint32_t diffCapture = 0;

uint32_t power_lock_enable = 0;

RTC_AlarmTypeDef sAlarm = {0};


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void collect_data(void) {
  //  HAL_UART_Init(&hlpuart1);
  /* tsl237_vdd_on(); */
  HAL_ADC_Init(&hadc1);
  write_sensor_data(&fs,read_vrefint(),read_temp(),tsl237_readsensor());
  /* tsl237_vdd_off(); */
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  switch(htim->Channel) {
  case HAL_TIM_ACTIVE_CHANNEL_1:
    //    HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET);
    captureDone = 1;
    tsl237_done = 1;
    // Handle Channel 1 Capture Event
    break;
  case HAL_TIM_ACTIVE_CHANNEL_2:
    /* HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET); */
    /* captureDone = 1; */
    /* tsl237t_done = 1; */
    // Handle Channel 1 Capture Event
    break;
  default:
    // Unhandled Capture Event 
    break;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // This callback is automatically called by the HAL on the UEV event
  if(htim->Instance == TIM2) {
    // Handle the timer overflow
    // This could be important for longer periods. 
    /* HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_SET); */
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  // This interrupt will go off every hour 
  collect_data_flag = 1;
}
  

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */
  enum {ON, OFF};
  uint8_t command[MAX_COMMAND_LEN];
  int command_length = 0;
  /* int i; */
  /* caldata_t cd; */
  /* caldata_t *cal_array = (caldata_t *) CAL_START; */
  /* int flash_error = 0; */
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  init_queue(&rx_queue);

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_ADC1_Init();          // 50uA 
  //   MX_DAC1_Init();         // Pull out later
  MX_LPUART1_UART_Init();
  MX_TIM2_Init();          // 120uA 
  RetargetInit(&hlpuart1);                        // Allow printf to work properly
  SysTick_Config(SystemCoreClock/TICK_FREQ_HZ);   // Start systick rolling
  //  led_on();
  HAL_Delay(600);
  //  led_off();
  //   sensor_power(POWER_ON);// Pull out later
  //  tsl237_vdd_off();           // Turn off the sensor power
  //  tsl237_vdd_on();

  //  HAL_DAC_DeInit(&hdac1);         
  HAL_ADC_DeInit(&hadc1);      // Kick off the A2D Subsystem
  HAL_TIM_Base_DeInit(&htim2);
  HAL_TIM_IC_DeInit(&htim2);

  // Pull out later
  //  HAL_DAC_Start(&hdac1,DAC_CHANNEL_2);
  //   HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0x7FF);
  // Pull out later
  
#ifdef DEPLOY
#warning "Disabling Processor Debugger Interface by Default"
  HAL_DBGMCU_DisableDBGStopMode();
#endif
  
  while (1) {
    printf("\n\r\n\rIU Dark Sky Light Sensor\n\r");
    printf("Version: %s\n\r",VERSION);
    printf("************************\n\r");
    /* print_option_bytes(); */
    /* printf("************************\n\r"); */
    flash_write_init(&fs);
    write_log_data(&fs,"Note: Cold Reset");
    cal_f2r();  // Copy calibration from flash to ram array. 
    prompt();

    //     while (1);
    
    while (1) {
      // Command Interpreter
      if (get_command(command)) {
        command_length = delspace(command);
        if (command_length != -1) {
          if(execute_command(command)) {
            printf("NOK\n\r");
          }
        }
        else {
          printf("NOK\n\r");
        }
        prompt();
      }
      // Automatic Data Collection
      // The collection is triggered by the RTC alarm.
      // If the power lock is enabled, then the sampling is blocked.
      // This modality is used in the sky command for the light source
      // mode of operation. 
      if ((collect_data_flag) && (!power_lock_enable)) {
        collect_data_flag = 0;
        HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BIN);
        // Only collect data between EVENING_START_HOUR and MORNING_END_HOUR
        if (((current_time.Hours >= EVENING_START_HOUR) && (current_time.Hours <= 23)) ||
             ((current_time.Hours >= 0) && (current_time.Hours <= MORNING_END_HOUR))) {
          sample();
        }
      }
      if (!power_lock_enable) {
        // Make sure that the DAC is off and go to stop 2 mode
        lp_stop_wfi();
      }
    }
  }
} 

/* 1 Mhz Version */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}


/* 80 Mhz Version */
/* /\** */
/*   * @brief System Clock Configuration */
/*   * @retval None */
/*   *\/ */
/* void SystemClock_Config(void) */
/* { */
/*   RCC_OscInitTypeDef RCC_OscInitStruct = {0}; */
/*   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; */
/*   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0}; */

/*   /\** Initializes the CPU, AHB and APB busses clocks */
/*   *\/ */
/*   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI; */
/*   RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS; */
/*   RCC_OscInitStruct.MSIState = RCC_MSI_ON; */
/*   RCC_OscInitStruct.MSICalibrationValue = 0; */
/*   RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6; */
/*   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; */
/*   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI; */
/*   RCC_OscInitStruct.PLL.PLLM = 1; */
/*   RCC_OscInitStruct.PLL.PLLN = 40; */
/*   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7; */
/*   RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; */
/*   RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; */
/*   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) */
/*   { */
/*     Error_Handler(); */
/*   } */
/*   /\** Initializes the CPU, AHB and APB busses clocks */
/*   *\/ */
/*   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK */
/*                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2; */
/*   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; */
/*   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; */
/*   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; */
/*   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; */

/*   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) */
/*   { */
/*     Error_Handler(); */
/*   } */
/*   PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LPUART1 */
/*                               |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3 */
/*                               |RCC_PERIPHCLK_ADC; */
/*   PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_LSE; */
/*   PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1; */
/*   PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1; */
/*   PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1; */
/*   PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1M = 1; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1N = 16; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2; */
/*   PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK; */
/*   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) */
/*   { */
/*     Error_Handler(); */
/*   } */
/*   /\** Configure the main internal regulator output voltage */
/*   *\/ */
/*   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) */
/*   { */
/*     Error_Handler(); */
/*   } */
/*   /\** Enable MSI Auto calibration */
/*   *\/ */
/*   HAL_RCCEx_EnableMSIPLLMode(); */
/* } */

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
  hlpuart1.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */
  LL_APB1_GRP2_EnableClockStopSleep(LL_APB1_GRP2_PERIPH_LPUART1);
  LL_LPUART_EnableClockInStopMode(LPUART1);
  LL_LPUART_SetWakeUpMethod(LPUART1, LL_LPUART_WAKEUP_ON_RXNE);
  //  LL_LPUART_SetWakeUpMethod(LPUART1,LL_LPUART_WAKEUP_ON_STARTBIT);
  LL_LPUART_EnableInStopMode (LPUART1);
  LL_LPUART_EnableIT_RXNE(LPUART1);
  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};
  /* RTC_TimeTypeDef current_time; */
  /* RTC_DateTypeDef current_date; */
  
  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  HAL_RTC_GetTime(&hrtc,&current_time,RTC_FORMAT_BCD);
  HAL_RTC_GetDate(&hrtc,&current_date,RTC_FORMAT_BCD);

  if (current_date.Year == 0) {
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x14;
    sDate.Year = 0x20;
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
      {
        Error_Handler();
      }
    sTime.Hours = 0x14;
    sTime.Minutes = 0x29;
    sTime.Seconds = 0x30;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
      {
        Error_Handler();
      }
    /** Enable the Alarm A 
     */
  }
  sAlarm.Alarm = RTC_ALARM_A;
  sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  sAlarm.AlarmMask =  RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS; // Match on specific minute and second 
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  sAlarm.AlarmTime.Hours = 0; 
  // Alarm will go off every hour at the following minute and second. 
  sAlarm.AlarmTime.Minutes = 0; 
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0x56;

  /* if (HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A)) { */
  /*   Error_Handler(); */
  /* } */
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) {
    Error_Handler();
  }
  
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  //  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK) */
  /* { */
  /*   Error_Handler(); */
  /* } */
  /* USER CODE BEGIN TIM2_Init 2 */
  /* HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0); //Set the priority of the timer to highest (0) */
  /* HAL_NVIC_EnableIRQ(TIM2_IRQn);  // Enable the IRQ in the NVIC */
  /* HAL_TIM_Base_Start_IT(&htim2);   // Turn on the IRQ in the timer */
  /* HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1); // Turn on the IRQ for CH1 input capture */
  /* HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2); // Turn on the IRQ for CH2 input capture  */
  /* USER CODE END TIM2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  //  HAL_GPIO_WritePin(GPIOA, sm_237t_pwr_Pin|tsl237_pwr_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, tsl237_pwr_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  /* HAL_GPIO_WritePin(led_out_GPIO_Port, led_out_Pin, GPIO_PIN_RESET); */

  /*Configure GPIO pin Output Level */
  //  HAL_GPIO_WritePin(sensor_power_GPIO_Port, sensor_power_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : rtc_EVI_Pin */
  /* GPIO_InitStruct.Pin = rtc_EVI_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* HAL_GPIO_Init(rtc_EVI_GPIO_Port, &GPIO_InitStruct); */

  /*Configure GPIO pins : sm_237t_pwr_Pin tsl237_pwr_Pin sensor_power_Pin */
  //   GPIO_InitStruct.Pin = sm_237t_pwr_Pin|tsl237_pwr_Pin|sensor_power_Pin;
  //  GPIO_InitStruct.Pin = tsl237_pwr_Pin|sensor_power_Pin;
  GPIO_InitStruct.Pin = tsl237_pwr_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : rtc_nINT_Pin sensor_int_Pin */
  /* GPIO_InitStruct.Pin = rtc_nINT_Pin|sensor_int_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); */

  /*Configure GPIO pin : led_out_Pin */
  /* GPIO_InitStruct.Pin = led_out_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; */
  /* HAL_GPIO_Init(led_out_GPIO_Port, &GPIO_InitStruct); */

  /* // Initialize the Range Shunts for the precision LED driver.  */
  /* GPIO_InitStruct.Pin = irange_0_Pin|irange_1_Pin|irange_2_Pin|irange_3_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; */
  /* HAL_GPIO_Init(led_out_GPIO_Port, &GPIO_InitStruct); */
  
  /* Configure GPIO pins : PB1 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|irange_0_Pin|irange_1_Pin|irange_2_Pin|irange_3_Pin; // Removed 5, 6, and 7. They are the range pins. 
  //  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7; 
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Turn on the brightest LED as a test. 
  /* GPIO_InitStruct.Pin = irange_1_Pin; */
  /* GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; */
  /* GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /* GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; */
  /* HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); */

  /*Configure GPIO pins : PA12 PA15 */
  GPIO_InitStruct.Pin = batt_measure_Pin|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PH3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
void MX_DAC1_Init(void) {

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */
  /** DAC Initialization 
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config 
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  if (HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END DAC1_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
