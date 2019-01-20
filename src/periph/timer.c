/*
 * Timer44kHz.c
 *
 *  Created on: May 17, 2015
  *      Author: karruzz
 *     License: GNU GPL 3
 */

#include "timer.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

// 44khz
void timer_init()
{
	// enable timer clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	// timer trigger init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	TIM_TimeBaseStructInit (&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler	= SystemCoreClock / 220000 - 1; // 0..108
	TIM_TimeBaseStructure.TIM_Period = 5 - 1; // 0..4
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3 , &TIM_TimeBaseStructure);

	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	// Enable Timer
	TIM_Cmd(TIM3 , ENABLE);
}

