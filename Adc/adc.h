/*
 * adc.h
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#include "stm32f10x.h"
#include "LowPass.h"

#define AUDIOBUFSIZE 256

uint16_t InBuf [AUDIOBUFSIZE];

int Sample;
//char FilterOn;

void SoundRecordInit(void);

#endif /* ADC_ADC_H_ */
