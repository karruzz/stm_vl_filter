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

#define AUDIOBUFSIZE 256

uint16_t OutBuf [AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;

char IsPlaying;

void SoundPlayerInit(void);
void SoundPlayerStart(void);

#endif /* DAC_DAC_H_ */
