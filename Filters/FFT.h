/*
 * FFT.h
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#ifndef FILTERS_FFT_H_
#define FILTERS_FFT_H_

#include "stm32f10x.h"

#define FFTSIZE 128
#define M 7

int16_t ReSignal[FFTSIZE];
int16_t ImSignal[FFTSIZE];
uint16_t Magnitude[FFTSIZE];

void FFTCalculate(void);

#endif /* FILTERS_FFT_H_ */
