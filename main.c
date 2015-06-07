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

/*
#include "FreeRTOS.h"
#include "task.h"
*/
//#include "queue.h"

int audioplayerHalf;
int audioplayerWhole;

char hist[MAX_X];

/*void Filtering(void *pvParameters )
{
	int filteringSample = 0;
    for (;;) {
		while (Sample - filteringSample > 3 || (filteringSample > Sample && Sample + AUDIOBUFSIZE - filteringSample > 3))
		{
			//OutBuf[filteringSample] = FilterLP(InBuf[filteringSample]);
			OutBuf[filteringSample] = InBuf[filteringSample];
			filteringSample = (++filteringSample) % AUDIOBUFSIZE;
	    	if (filteringSample > AUDIOBUFSIZE / 2) RecordingHalf = HIGH; else RecordingHalf = LOW;
		}
    }
}*/

#define WINDOW 8
#define DEVIDER (AUDIOBUFSIZE - 1)

int16_t h[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

extern unsigned int FIR(uint16_t* inbuff, int16_t* hbuf,int position);

static uint8_t outBuffer[MAX_X * MAX_Y_OCTETS];
static inline void Nokia5110PrepareHist(const uint8_t *pointY)
{
	int i;
	int j;
	int boundary;

	for(j = 0; j < MAX_X; j++)
	{
		boundary = MAX_Y_OCTETS - 1 - (pointY[j] >> 3);
		for(i = 0; i < MAX_Y_OCTETS; i++)
		{
			if(i < boundary) outBuffer[j+i*MAX_X] = 0;
			else if (i == boundary) outBuffer[j+i*MAX_X] = ~(0xFF >> (pointY[j] & 7));
			else outBuffer[j+i*MAX_X] = 0xFF;
		}
	}
}

int main(void)
{
	int i;
	unsigned int filteringSample = 0;
	int displayUpdateCounter;
	uint16_t x;
	Boolean FFTReady = FALSE;
	Boolean HistReady = FALSE;

	Nokia5110Init();

	SoundRecorderInit();
	SoundPlayerInit();
	Timer44kHzInit();

	while (Sample < PLAYERDELAY);
	SoundPlayerStart();

/*    xTaskCreate(Filtering, (signed char*)"filtering", 500, NULL, tskIDLE_PRIORITY + 4, NULL);

    vTaskStartScheduler();*/

	while(1) {
		while (((Sample - filteringSample) & DEVIDER) > 10)
		{
			OutBuf[filteringSample] = FIR(InBuf, h, filteringSample);
		//	OutBuf[filteringSample] = InBuf[filteringSample];
			filteringSample = (++filteringSample) & DEVIDER;
		}

/*		displayUpdateCounter++;
		if (displayUpdateCounter < 5000) continue;
		displayUpdateCounter = 0;*/

		if (FFTReady == FALSE)
		{
			i = 0;
			x = filteringSample;
			while (i++ < FFTSIZE) {
				FFTAdd(OutBuf[x]);
				x = (--x) & DEVIDER;
			}
			FFTCalculate();
			FFTReady = TRUE;
			continue;

/*			i = 0;
			x = filteringSample;
			while (i < 128) {
				Magnitude[i++] = OutBuf[x] >> 7;
				x = (--x) & DEVIDER;
			}
			FFTReady = TRUE;
			FFTIsFull = FALSE;
			FFTsample = 0;
			continue;*/
		}

		if (HistReady == FALSE)
		{
			FFTComputeMagnitude();
			Nokia5110PrepareHist(Magnitude);
			HistReady = TRUE;
			continue;
		}

		FFTReady = FALSE;
		HistReady = FALSE;
		Nokia5110DrawHist(outBuffer);
	}
}

