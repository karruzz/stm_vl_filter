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

int audioplayerHalf;
int audioplayerWhole;

char hist[MAX_X];

#define WINDOW 8
#define DEVIDER (AUDIOBUFSIZE - 1)

//int16_t h[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//вч, делить на 128
//int16_t h[] = { 0, 0, 0, 1, 3, -1, -16, -33, 86, -28, -12, -1, 1, 0, 0, 0};
//нч, делить на 128
int16_t h[] = { 0, 0, 0, 0, 3, 8, 16, 22, 23, 19, 12, 5, 1, 0, 0, 0};

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

static void ButtonLedInit()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructureA;
	GPIO_StructInit (&GPIO_InitStructureA);
	GPIO_InitStructureA.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructureA);

    GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_StructInit (&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main(void)
{
	int i;
	unsigned int filteringSample = 0;
	int displayUpdateCounter;
	uint16_t x;
	Boolean FFTReady = FALSE;
	Boolean HistReady = FALSE;
	Boolean IsFilterOn = FALSE;

	ButtonLedInit();
	Nokia5110Init();

	SoundRecorderInit();
	SoundPlayerInit();
	Timer44kHzInit();

	while (Sample < PLAYERDELAY);
	SoundPlayerStart();

	while(1) {
		while (((Sample - filteringSample) & DEVIDER) > 10)
		{
			OutBuf[filteringSample] = IsFilterOn == TRUE ? FIR(InBuf, h, filteringSample)
														 : InBuf[filteringSample];
			filteringSample = (++filteringSample) & DEVIDER;
		}


		if (GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_0) > 0)
		{
			i = 100;
			while (i-- > 0) asm("nop");
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) > 0)
			{
				if (IsFilterOn == TRUE)
				{
					IsFilterOn = FALSE;
					GPIO_ResetBits(GPIOC, GPIO_Pin_8);
				}
				else
				{
					IsFilterOn = TRUE;
					GPIO_SetBits(GPIOC, GPIO_Pin_8);
				}
			}
			continue;
		}


		if (IsFilterOn == FALSE)
		{
			displayUpdateCounter++;
			if (displayUpdateCounter < 2000) continue;
			displayUpdateCounter = 0;
		}

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

