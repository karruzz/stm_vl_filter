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

int16_t factors[] = { 2, 2, 2, 2, 2, 2, 2, 2 };

static inline uint16_t FIRFilter1(int position)
{
	int i = 0;
	int sum = 0;
	while (i < WINDOW)
	{
		sum += InBuf[(position--) & DEVIDER] * factors[i++];
	}

	return sum >> 4;
}

static char boundaryHist[MAX_X];
static char outBuffer[MAX_X * MAX_Y_OCTETS];

static inline void Nokia5110PrepareHist(const char *pointY)
{
	int i;
	int j;

	for(i = 0; i < MAX_X; i++)
		boundaryHist[i] = MAX_Y / 8 - 1 - pointY[i] / 8;

	for(i = 0; i < MAX_Y_OCTETS; i++)
		for(j = 0; j < MAX_X; j++){
		{
			if(i < boundaryHist[j]) outBuffer[j+i*MAX_X] = 0;
			else if (i == boundaryHist[j]) outBuffer[j+i*MAX_X] = ~(0xFF >> (pointY[j] % 8));
			else outBuffer[j+i*MAX_X] = 0xFF;
		}
	}
}

int main(void)
{
//	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

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
			OutBuf[filteringSample] = FIRFilter1(filteringSample);
			OutBuf[filteringSample] = InBuf[filteringSample];
			filteringSample = (++filteringSample) & DEVIDER;
		}

/*		displayUpdateCounter++;
		if (displayUpdateCounter < 5000) continue;
		displayUpdateCounter = 0;*/

		if (FFTReady == FALSE)
		{
			x = filteringSample;
			while (FFTIsFull == FALSE) {
				FFTAdd(OutBuf[x] >> 4);
				x = (--x) & DEVIDER;
			}
			FFTCalculate();
			FFTReady = TRUE;
			continue;
		}

		if (HistReady == FALSE)
		{
			Nokia5110PrepareHist(Magnitude);
			HistReady = TRUE;
			continue;
		}

		FFTReady = FALSE;
		HistReady = FALSE;
		Nokia5110DrawHist(outBuffer);
	}
}

