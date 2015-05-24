/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "Nokia5110.h"
#include "Timer44kHz.h"
#include "adc.h"
#include "dac.h"
#include "FFT.h"
#include "LowPass.h"

typedef enum { LOW, HIGH } Half;
Half RecordingHalf;

int audioplayerHalf;
int audioplayerWhole;

char hist[MAX_X];

int main(void)
{
	int i;
	int filteringSample = 0;
	int displayUpdateCounter;
	int left;

	Half lastHalf;
	Nokia5110Init();

	SoundRecordInit();
	SoundPlayerInit();
	Timer44kHzInit();

	while(1) {
		if (IsPlaying == 0 && filteringSample > AUDIOBUFSIZE / 2) SoundPlayerStart();

		while (Sample - filteringSample > 3 || (filteringSample > Sample && Sample + 256 - filteringSample > 3))
		{
			//OutBuf[filteringSample] = FilterLP(InBuf[filteringSample]);
			OutBuf[filteringSample] = InBuf[filteringSample];
			filteringSample = (++filteringSample) % AUDIOBUFSIZE;
	    	if (filteringSample > AUDIOBUFSIZE / 2) RecordingHalf = HIGH; else RecordingHalf = LOW;
		}

		displayUpdateCounter++;
		if (displayUpdateCounter < 10000 || lastHalf == RecordingHalf) continue;

		displayUpdateCounter = 0;
		if (RecordingHalf == HIGH) left = 0; else left = AUDIOBUFSIZE / 2;

		for (i = 0; i < FFTSIZE; i++)
		{
			ReSignal[i] = OutBuf[i + left] >> 5;
			ImSignal[i] = 0;
		}

		for (i = 0; i < 64; i++)
		{
			hist[i] = Magnitude[i]>>3;
		}

		FFTCalculate();
		Nokia5110DrawHist(hist);
		lastHalf = RecordingHalf;
	}
}

