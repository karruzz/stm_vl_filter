/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#include "misc.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

#include "nokia_5110.h"
#include "timer.h"
#include "adc.h"
#include "dac.h"
#include "fft.h"

int audioplayerHalf;
int audioplayerWhole;

char hist[MAX_X];

#define WINDOW 8
#define DEVIDER (AUDIOBUFSIZE - 1)

static uint8_t screen_buffer[MAX_X * MAX_Y_OCTETS];
static uint8_t test_out[MAX_X];

//int16_t h[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
// high pass filer, devide on 128
//int16_t h[] = { 0, 0, 0, 1, 3, -1, -16, -33, 86, -28, -12, -1, 1, 0, 0, 0};
// low pass filer, devide on 128
int16_t h[] = { 0, 0, 0, 0, 3, 8, 16, 22, 23, 19, 12, 5, 1, 0, 0, 0};

static uint16_t FIR(uint16_t* inbuff, int16_t* hbuf, int position)
{
	int sum = 0;
	for (int i = 0; i < 16; ++i, position = (position - 1) & DEVIDER)
		sum += hbuf[i] * inbuff[position];

	return sum >> 7;
}

static void prepare_histogramm(const uint8_t *points)
{
	for(int x = 0; x < MAX_X; x++)
	{
		int value = MAX_Y_OCTETS - 1 - (points[x] >> 3);
		for(int y = 0; y < MAX_Y_OCTETS; y++)
		{
			int point = x + y * MAX_X;
			if(y < value) screen_buffer[point] = 0;
			else if (y == value) screen_buffer[point] = ~(0xFF >> (points[x] & 0x07));
			else screen_buffer[point] = 0xFF;
		}
	}
}

static void button_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructureA;
	GPIO_StructInit (&GPIO_InitStructureA);
	GPIO_InitStructureA.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructureA);

	// Configure Interrupt test
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable Interrupt in NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void led_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static int filter_on;

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		EXTI_ClearITPendingBit(EXTI_Line0);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) > 0) {
			filter_on ? GPIO_ResetBits(GPIOC, GPIO_Pin_8) : GPIO_SetBits(GPIOC, GPIO_Pin_8);
			filter_on = 1 - filter_on;
		}
		int i = 500;
		while (--i > 0) asm("nop");
	}
}

int main(void)
{
	int i, update_screen_counter = 0;
	uint16_t x, filtering_sample = 0;

	bool fft_ready = FALSE, hist_ready = FALSE;

	led_init();
	button_init();
	nokia_5110_init();
	nokia_5110_clear();
	sound_recorder_init();
	sound_player_init();
	timer_init();

	while (get_current_sample() < PLAYERDELAY);

	sound_player_start();

	while(1) {
		while (((get_current_sample() - filtering_sample) & DEVIDER) > 10) {
			output_buffer[filtering_sample] =
				filter_on == TRUE ? FIR(input_buffer, h, filtering_sample) : input_buffer[filtering_sample];
			filtering_sample = (++filtering_sample) & DEVIDER;
		}

		if (++update_screen_counter < 5000) continue;
		update_screen_counter = 0;

		if (fft_ready == FALSE) {
			i = 0;
			x = filtering_sample;
			while (i++ < FFTSIZE) {
				FFTAdd(output_buffer[x]);
				x = (--x) & DEVIDER;
			}
			FFTCalculate();
			fft_ready = TRUE;
			continue;
		}

		if (hist_ready == FALSE) {
			FFTComputeMagnitude();
			prepare_histogramm(Magnitude);
			hist_ready = TRUE;
			continue;
		}

		fft_ready = FALSE;
		hist_ready = FALSE;
		nokia_5110_hist(screen_buffer);
	}
}