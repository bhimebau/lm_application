/* tsl25911.h
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 02.17.2019
 * Copyright (C) 2019
 *
 */

#ifndef TSL25911_H
#define TSL25911_H

#define TSL25911_ADDR        (0x29)
#define TSL25911_ID          (0x50)

#define TSL25911_CMD_BIT     (0xA0)

#define TSL25911_EN_NPIEN    (0x80)
#define TSL25911_EN_SAI      (0x40)
#define TSL25911_EN_AIEN     (0x10)
#define TSL25911_EN_AEN      (0x02)
#define TSL25911_EN_PON      (0x01)
#define TSL25911_EN_POFF     (0x00)

#define TSL25911_LUX_DF      (408.0F)
#define TSL25911_LUX_COEFB   (1.64F)  // CH0 coefficient 
#define TSL25911_LUX_COEFC   (0.59F)  // CH1 coefficient A
#define TSL25911_LUX_COEFD   (0.86F)  // CH2 coefficient B

enum {
    TSL25911_REG_ENABLE          = 0x00,
    TSL25911_REG_CONTROL         = 0x01,
    TSL25911_REG_THRES_AILTL     = 0x04,
    TSL25911_REG_THRES_AILTH     = 0x05,
    TSL25911_REG_THRES_AIHTL     = 0x06,
    TSL25911_REG_THRES_AIHTH     = 0x07,
    TSL25911_REG_THRES_NPAILTL   = 0x08,
    TSL25911_REG_THRES_NPAILTH   = 0x09,
    TSL25911_REG_THRES_NPAIHTL   = 0x0A,
    TSL25911_REG_THRES_NPAIHTH   = 0x0B,
    TSL25911_REG_PERSIST         = 0x0C,
    TSL25911_REG_PID             = 0x11,
    TSL25911_REG_ID              = 0x12,
    TSL25911_REG_STATUS          = 0x13,
    TSL25911_REG_CHAN0_L         = 0x14,
    TSL25911_REG_CHAN0_H         = 0x15,
    TSL25911_REG_CHAN1_L         = 0x16,
    TSL25911_REG_CHAN1_H         = 0x17,
};

typedef enum {
    TSL25911_GAIN_LOW    = 0x00,
    TSL25911_GAIN_MED    = 0x01,
    TSL25911_GAIN_HIGH   = 0x02,
    TSL25911_GAIN_MAX    = 0x03,
} tsl25911Gain_t;

typedef enum {
    TSL25911_INTT_100MS  = 0x00,
    TSL25911_INTT_200MS  = 0x01,
    TSL25911_INTT_300MS  = 0x02,
    TSL25911_INTT_400MS  = 0x03,
    TSL25911_INTT_500MS  = 0x04,
    TSL25911_INTT_600MS  = 0x05,
} tsl25911IntegrationTime_t;

typedef enum {
    TSL25911_PER_EVERY   = 0x00,
    TSL25911_PER_ANY     = 0x01,
    TSL25911_PER_2       = 0x02,
    TSL25911_PER_3       = 0x03,
    TSL25911_PER_5       = 0x04,
    TSL25911_PER_10      = 0x05,
    TSL25911_PER_15      = 0x06,
    TSL25911_PER_20      = 0x07,
    TSL25911_PER_25      = 0x08,
    TSL25911_PER_30      = 0x09,
    TSL25911_PER_35      = 0x0A,
    TSL25911_PER_40      = 0x0B,
    TSL25911_PER_45      = 0x0C,
    TSL25911_PER_50      = 0x0D,
    TSL25911_PER_55      = 0x0E,
    TSL25911_PER_60      = 0x0F,
} tsl25911Persist_t;

typedef struct tsl25911_shadow {
  I2C_HandleTypeDef           *i2c_port;
  tsl25911Gain_t               gain;
  tsl25911IntegrationTime_t    integration;
  uint32_t rawALS;
  uint32_t full;
  uint32_t ir;
  uint32_t visible;
  float lux;
  int saturated;
} tsl25911_shadow_t;

#endif

void tsl25911_vdd_on(void);
void tsl25911_vdd_off(void);
uint8_t tsl25911_readID(tsl25911_shadow_t *);
uint8_t tsl25911_readControl(tsl25911_shadow_t *);
int tsl25911_init(tsl25911_shadow_t *,
                   I2C_HandleTypeDef *,
                   tsl25911Gain_t,
                   tsl25911IntegrationTime_t);

int tsl25911_setGain(tsl25911_shadow_t *, tsl25911Gain_t);
int tsl25911_setIntg(tsl25911_shadow_t *, tsl25911IntegrationTime_t);
void tsl25911_enable(tsl25911_shadow_t *);
void tsl25911_disable(tsl25911_shadow_t *);
void tsl25911_getALS(tsl25911_shadow_t *);
void tsl25911_calcLux(tsl25911_shadow_t *);
float tsl25911_readsensor(I2C_HandleTypeDef *);


