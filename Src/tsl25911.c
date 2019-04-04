/* tls25911.c
 * Description: Functions used to communicate with the TLS25911 light sensor. 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#include "main.h"
#include <stdio.h>
#include "tsl25911.h"

// The power supply VSW must be enabled through the SW_MODE pin before the TSL25911 will be
// powered.

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

/*
 *  Read ALS
 *  Read full spectrum, infrared, and visible
 */
/* void TSL2591::getALS(void) */
/* { */
/*     enable(); */
/*     for(uint8_t t=0; t<=_integ+1; t++) { */
/*         wait(0.12); */
/*     } */
/*     char write1[] = {(TSL2591_CMD_BIT|TSL2591_REG_CHAN1_L)}; */
/*     _i2c.write(_addr, write1, 1, 0); */
/*     char read1[2]; */
/*     _i2c.read(_addr, read1, 2, 0); */
/*     char write2[] = {(TSL2591_CMD_BIT|TSL2591_REG_CHAN0_L)}; */
/*     _i2c.write(_addr, write2, 1, 0); */
/*     char read2[2]; */
/*     _i2c.read(_addr, read2, 2, 0); */
/*     rawALS = (((read1[1]<<8)|read1[0])<<16)|((read2[1]<<8)|read2[0]); */
/*     disable(); */
/*     full = rawALS & 0xFFFF; */
/*     ir = rawALS >> 16; */
/*     visible = full - ir; */
/* } */
/* /\* */
/*  *  Calculate Lux */
/*  *\/ */
/* void TSL2591::calcLux(void) */
/* { */
/*     float atime, again, cpl, lux1, lux2, lux3; */
/*     if((full == 0xFFFF)|(ir == 0xFFFF)) { */
/*         lux3 = 0; */
/*         return; */
/*     } */
/*     switch(_integ) { */
/*         case TSL2591_INTT_100MS: */
/*             atime = 100.0F; */
/*             break; */
/*         case TSL2591_INTT_200MS: */
/*             atime = 200.0F; */
/*             break; */
/*         case TSL2591_INTT_300MS: */
/*             atime = 300.0F; */
/*             break; */
/*         case TSL2591_INTT_400MS: */
/*             atime = 400.0F; */
/*             break; */
/*         case TSL2591_INTT_500MS: */
/*             atime = 500.0F; */
/*             break; */
/*         case TSL2591_INTT_600MS: */
/*             atime = 600.0F; */
/*             break; */
/*         default: */
/*             atime = 100.0F; */
/*             break; */
/*     } */
/*     switch(_gain) { */
/*         case TSL2591_GAIN_LOW: */
/*             again = 1.0F; */
/*             break; */
/*         case TSL2591_GAIN_MED: */
/*             again = 25.0F; */
/*             break; */
/*         case TSL2591_GAIN_HIGH: */
/*             again = 428.0F; */
/*             break; */
/*         case TSL2591_GAIN_MAX: */
/*             again = 9876.0F; */
/*             break; */
/*         default: */
/*             again = 1.0F; */
/*             break; */
/*     } */
/*     cpl = (atime * again) / TSL2591_LUX_DF; */
/*     lux1 = ((float)full - (TSL2591_LUX_COEFB * (float)ir)) / cpl; */
/*     lux2 = (( TSL2591_LUX_COEFC * (float)full ) - ( TSL2591_LUX_COEFD * (float)ir)) / cpl; */
/*     lux3 = lux1 > lux2 ? lux1 : lux2; */
/*     lux = (uint32_t)lux3; */
/* } */


/* void TSL2591::setTime(tsl2591IntegrationTime_t integ) */
/* { */
/*     enable(); */
/*     _integ = integ; */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), (_integ|_gain)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/*     disable(); */
/* } */




/* void TSL2591::setGain(tsl2591Gain_t gain) */
/* { */
/*     enable(); */
/*     _gain = gain; */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), (_integ|_gain)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/*     disable(); */
/* } */

/* #include "TSL2591.h" */

/* TSL2591::TSL2591 (I2C& tsl2591_i2c, uint8_t tsl2591_addr): */
/*     _i2c(tsl2591_i2c), _addr(tsl2591_addr<<1) */
/* { */
/*     _init = false; */
/*     _integ = TSL2591_INTT_100MS; */
/*     _gain = TSL2591_GAIN_LOW; */
/* } */
/* /\* */
/*  *  Initialize TSL2591 */
/*  *  Checks ID and sets gain and integration time */
/*  *\/ */
/* bool TSL2591::init(void) */
/* { */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_ID)}; */
/*     if(_i2c.write(_addr, write, 1, 0) == 0) { */
/*         char read[1]; */
/*         _i2c.read(_addr, read, 1, 0); */
/*         if(read[0] == TSL2591_ID) { */
/*             _init = true; */
/*             setGain(TSL2591_GAIN_LOW); */
/*             setTime(TSL2591_INTT_100MS); */
/*             disable(); */
/*             return true; */
/*         } */
/*     } */
/*     return false; */
/* } */
/* /\* */
/*  *  Power On TSL2591 */
/*  *\/ */
/* void TSL2591::enable(void) */
/* { */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_ENABLE), (TSL2591_EN_PON|TSL2591_EN_AEN|TSL2591_EN_AIEN|TSL2591_EN_NPIEN)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/* } */
/* /\* */
/*  *  Power Off TSL2591 */
/*  *\/ */
/* void TSL2591::disable(void) */
/* { */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_ENABLE), (TSL2591_EN_POFF)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/* } */
/* /\* */
/*  *  Set Gain and Write */
/*  *  Set gain and write time and gain */
/*  *\/ */
/* void TSL2591::setGain(tsl2591Gain_t gain) */
/* { */
/*     enable(); */
/*     _gain = gain; */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), (_integ|_gain)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/*     disable(); */
/* } */
/* /\* */
/*  *  Set Integration Time and Write */
/*  *  Set gain and write time and gain */
/*  *\/ */
/* void TSL2591::setTime(tsl2591IntegrationTime_t integ) */
/* { */
/*     enable(); */
/*     _integ = integ; */
/*     char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), (_integ|_gain)}; */
/*     _i2c.write(_addr, write, 2, 0); */
/*     disable(); */
/* } */
