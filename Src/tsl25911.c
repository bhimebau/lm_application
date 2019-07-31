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
#include "tsl25911.h"

extern I2C_HandleTypeDef hi2c1;

void tsl25911_vdd_on(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = sensor_power_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
  
void tsl25911_vdd_off(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = sensor_power_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void light_command(char *arguments) {
  uint8_t rxbuf;

  if (arguments) {
    printf("NOK\n\r");
  }
  else {
    printf("%4.3f\n\r",tsl25911_readsensor(&hi2c1));
    HAL_I2C_Mem_Read(&hi2c1,
                     (TSL25911_ADDR<<1),
                     0xA0|TSL25911_REG_ENABLE,
                     1,
                     &rxbuf,
                     1,
                     HAL_MAX_DELAY
                     );
    
    /* printf("Light Sensor Control Reg = 0x%02x\n\r",rxbuf); */
    printf("OK\n\r");
  }
}

int tsl25911_writereg(I2C_HandleTypeDef *hi2c1, uint8_t addr, uint8_t * data, uint16_t size) {
  HAL_StatusTypeDef status;
  status = HAL_I2C_Mem_Write(hi2c1,
                            (TSL25911_ADDR<<1),
                            0xA0|addr,
                            1,
                            data,
                            size,
                            HAL_MAX_DELAY
                            );
  return((int) status);
}

int tsl25911_readreg(I2C_HandleTypeDef *hi2c1, uint8_t addr, uint8_t * data, uint16_t size) {
  HAL_StatusTypeDef status;
  status = HAL_I2C_Mem_Read(hi2c1,
                   (TSL25911_ADDR<<1),
                   0xA0|addr,
                   1,
                   data,
                   size,
                   HAL_MAX_DELAY
                   );
  return((int) status);
}

int tsl25911_init(tsl25911_shadow_t *shadow,
                   I2C_HandleTypeDef *port,
                   tsl25911Gain_t gain,
                   tsl25911IntegrationTime_t integration) {
  if ((gain<0)||(gain>3)) return (-1);
  if ((integration<0)||(integration>5)) return (-1);
  
  shadow->i2c_port = port;
  shadow->gain = gain;
  shadow->integration = integration;
  if (tsl25911_readID(shadow) != 0x50) {
    return (-1);
  }
  tsl25911_setGain(shadow,gain); 
  tsl25911_setIntg(shadow,integration);
  tsl25911_disable(shadow);
  return (0);
}

uint8_t tsl25911_readID(tsl25911_shadow_t *shadow) {
  uint8_t rxbuf;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_ID,&rxbuf,1);
  return rxbuf;
}

uint8_t tsl25911_readControl(tsl25911_shadow_t *shadow) {
  uint8_t rxbuf;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_CONTROL,&rxbuf,1);
  return rxbuf;
}

int tsl25911_setGain(tsl25911_shadow_t *shadow, tsl25911Gain_t gain) {
  uint8_t rxbuf;
  if ((gain<0) || (gain>0x03)) {
    return (-1);
  }
  shadow->gain = gain;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_CONTROL,&rxbuf,1);
  rxbuf &= ~0x30;
  rxbuf |= (gain<<4);
  tsl25911_writereg(shadow->i2c_port,TSL25911_REG_CONTROL,&rxbuf,1);
  return (0);
}

int tsl25911_setIntg(tsl25911_shadow_t *shadow, tsl25911IntegrationTime_t integration) {
  uint8_t rxbuf;
  if ((integration<0) || (integration>0x05)) {
    return (-1);
  }
  shadow->integration = integration;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_CONTROL,&rxbuf,1);
  rxbuf &= ~0x07;
  rxbuf |= integration;
  tsl25911_writereg(shadow->i2c_port,TSL25911_REG_CONTROL,&rxbuf,1);
  return (0);
}

void tsl25911_enable(tsl25911_shadow_t *shadow) {
  uint8_t rxbuf;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_ENABLE,&rxbuf,1);
  rxbuf |= TSL25911_EN_PON;
  rxbuf |= TSL25911_EN_AEN;
  tsl25911_writereg(shadow->i2c_port,TSL25911_REG_ENABLE,&rxbuf,1);
}

void tsl25911_disable(tsl25911_shadow_t *shadow) {
  uint8_t rxbuf;
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_ENABLE,&rxbuf,1);
  rxbuf &= ~TSL25911_EN_PON;
  rxbuf &= ~TSL25911_EN_AEN;
  tsl25911_writereg(shadow->i2c_port,TSL25911_REG_ENABLE,&rxbuf,1);
}

// ALS stands for ambient light sensor ...
void tsl25911_getALS(tsl25911_shadow_t *shadow) {
  uint8_t sensor_data[4];
  uint32_t start_time = uwTick; // uwTick increments every 10mS in systick
  uint32_t end_time;
  switch (shadow->integration) {
  case TSL25911_INTT_100MS:
    end_time = start_time + 10;
    break;
  case TSL25911_INTT_200MS:
    end_time = start_time + 20;
    break;
  case TSL25911_INTT_300MS:
    end_time = start_time + 30;
    break;
  case TSL25911_INTT_400MS:
    end_time = start_time + 40;
    break;
  case TSL25911_INTT_500MS:
    end_time = start_time + 50;
    break;
  case TSL25911_INTT_600MS:
    end_time = start_time + 60;
    break;
  default:
    end_time = start_time + 60;
  }
  tsl25911_enable(shadow);
  while (uwTick <= (end_time+5)); 
  tsl25911_readreg(shadow->i2c_port,TSL25911_REG_CHAN0_L,sensor_data,4);
  tsl25911_disable(shadow);
  shadow->rawALS = 0;
  shadow->rawALS = ((((sensor_data[3]<<8)|sensor_data[2])<<16)|((sensor_data[1]<<8)|sensor_data[0]));
  shadow->full = shadow->rawALS & 0xFFFF;
  shadow->ir = shadow->rawALS >> 16;
  shadow->visible = shadow->full - shadow->ir;
}

void tsl25911_calcLux(tsl25911_shadow_t *shadow) {
  float atime, again, cpl, lux1, lux2;
  shadow->saturated = 0;
  if((shadow->full == 0xFFFF)|(shadow->ir == 0xFFFF)) {
    shadow->saturated = 1;
    shadow->lux = 0;
    return;
  }
  switch(shadow->integration) {
  case TSL25911_INTT_100MS:
    atime = 100.0F;
    break;
  case TSL25911_INTT_200MS:
    atime = 200.0F;
    break;
  case TSL25911_INTT_300MS:
    atime = 300.0F;
    break;
  case TSL25911_INTT_400MS:
    atime = 400.0F;
    break;
  case TSL25911_INTT_500MS:
    atime = 500.0F;
    break;
  case TSL25911_INTT_600MS:
    atime = 600.0F;
    break;
  default:
    atime = 100.0F;
    break;
  }
  switch(shadow->gain) {
  case TSL25911_GAIN_LOW:
    again = 1.0F;
    break;
  case TSL25911_GAIN_MED:
    again = 25.0F;
    break;
  case TSL25911_GAIN_HIGH:
    again = 428.0F;
    break;
  case TSL25911_GAIN_MAX:
    again = 9876.0F;
    break;
  default:
    again = 1.0F;
    break;
  }
  cpl = (atime * again) / TSL25911_LUX_DF;
  lux1 = ((float)shadow->full - (TSL25911_LUX_COEFB * (float)shadow->ir)) / cpl;
  lux2 = (( TSL25911_LUX_COEFC * (float)shadow->full ) - ( TSL25911_LUX_COEFD * (float)shadow->ir)) / cpl;
  shadow->lux = lux1 > lux2 ? lux1 : lux2;
  shadow->saturated = 0;
}

float tsl25911_readsensor(I2C_HandleTypeDef *i2c_port) {
  tsl25911_shadow_t s;
  tsl25911_init(&s,i2c_port,TSL25911_GAIN_MAX,TSL25911_INTT_600MS);
  tsl25911_getALS(&s);
  tsl25911_calcLux(&s);
  while (s.saturated) {
    switch (s.gain) {
    case TSL25911_GAIN_MAX:
      tsl25911_init(&s,i2c_port,TSL25911_GAIN_HIGH,TSL25911_INTT_600MS);
      tsl25911_getALS(&s);
      tsl25911_calcLux(&s);
      break;
    case TSL25911_GAIN_HIGH:
      tsl25911_init(&s,i2c_port,TSL25911_GAIN_MED,TSL25911_INTT_600MS);
      tsl25911_getALS(&s);
      tsl25911_calcLux(&s);
      break;
    case TSL25911_GAIN_MED:
      tsl25911_init(&s,i2c_port,TSL25911_GAIN_LOW,TSL25911_INTT_600MS);
      tsl25911_getALS(&s);
      tsl25911_calcLux(&s);
      break;
    case TSL25911_GAIN_LOW:
      return (-1);
      break;
    }
  }
  return s.lux;
}
