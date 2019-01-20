/*
 * dac.h
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#ifndef DAC_DAC_H_
#define DAC_DAC_H_

#include "stm32f10x.h"
#include "config.h"

uint16_t output_buffer [AUDIOBUFSIZE];

void sound_player_init(void);
void sound_player_start(void);

#endif /* DAC_DAC_H_ */
