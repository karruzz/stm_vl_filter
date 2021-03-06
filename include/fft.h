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
#include "config.h"

int16_t ReSignal[FFTSIZE];
int16_t ImSignal[FFTSIZE];
uint8_t Magnitude[FFTSIZE];

unsigned int FFTsample;

void FFTCalculate(void);
void FFTAdd(int);
void FFTComputeMagnitude(void);

#endif /* FILTERS_FFT_H_ */

