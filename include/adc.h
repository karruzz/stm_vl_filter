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
#include "config.h"

// #include "FreeRTOS.h"
// #include "task.h"

uint16_t input_buffer [AUDIOBUFSIZE];

uint16_t get_current_sample();
void sound_recorder_init(void);

#endif /* ADC_ADC_H_ */
