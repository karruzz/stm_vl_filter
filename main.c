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

int main(void)
{
//	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	int i;
	int filteringSample = 0;
	int displayUpdateCounter;
	uint16_t x;
	Boolean GoingToView = FALSE;
	FFTIsFull = FALSE;
	FFTIsMagnitudeReady = FALSE;

	Nokia5110Init();

	SoundRecorderInit();
	SoundPlayerInit();
	Timer44kHzInit();

	while (Sample < PLAYERDELAY);
	SoundPlayerStart();

/*    xTaskCreate(Filtering, (signed char*)"filtering", 500, NULL, tskIDLE_PRIORITY + 4, NULL);

    vTaskStartScheduler();*/

	while(1) {
		while (Sample - filteringSample > 3 || (filteringSample > Sample && Sample + AUDIOBUFSIZE - filteringSample > 3))
		{
			//OutBuf[filteringSample] = FilterLP(InBuf[filteringSample]);
			x = InBuf[filteringSample];
			OutBuf[filteringSample] = x;
			if (FFTIsFull == FALSE && GoingToView == FALSE) FFTAdd(x >> 5);
			filteringSample = (++filteringSample) % AUDIOBUFSIZE;
		}

		displayUpdateCounter++;
		if (displayUpdateCounter < 5000) continue;
		displayUpdateCounter = 0;

		if (FFTIsFull == TRUE) {
			FFTCalculate();
			GoingToView = TRUE;
			continue;
		}

		Nokia5110DrawHist(Magnitude);
		GoingToView = FALSE;
	}
}

