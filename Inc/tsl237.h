/* tsl237.c
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 09.11.2019
 * Copyright (C) 2019
 *
 */

#ifndef TSL237_H
#define TSL237_H



#endif

extern uint32_t tsl237_done;
extern uint32_t tsl237t_done;

void tsl237_vdd_on(void);
void tsl237_vdd_off(void);
void tsl237_command(char *);
// float tsl237_readsensor(void);
uint32_t tsl237_readsensor(void); 
