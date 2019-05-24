/* pwr.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 05.23.2019
 * Copyright (C) 2019
 *
 */

/**
  * @brief  Configures system clock after wake-up from STOP: enable MSI, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  uint32_t pFLatency = 0;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Get the Oscillators configuration according to the internal RCC registers */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  /* Enable PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get the Clocks configuration according to the internal RCC registers */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief Configures system clock after wake-up from STOP: enable MSI, PLL, ADC1
 *  and select PLL as system clock source.
 * @param None
 * @retval None
 */
static void SYSCLKConfig_STOP(void)
{
 RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
 uint32_t pFLatency = 0;
 /* Enable Power Control clock */
 __HAL_RCC_PWR_CLK_ENABLE();
 /* Get the Oscillators configuration according to the internal RCC registers */
 HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
 /* Enable PLL */
 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
 {
   Error_Handler();
 }
 /* Get the Clocks configuration according to the internal RCC registers */
 HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);
 /* Select PLL as system clock source and keep HCLK, PCLK1 and PCLK2 clocks dividers as before */
 RCC_ClkInitStruct.ClockType    = RCC_CLOCKTYPE_SYSCLK;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
 {
   Error_Handler();
 }
 /* ENABLE ADC1 source clock */
 PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
 PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
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
}

