/*
 * adc.c
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#include "adc.h"

#include "misc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define WINDOW 4
#define DEVISOR 4

// adc pin PA2
void sound_recorder_init()
{
	// adc init
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);

	//adc interrupt init
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitTypeDef NVIC_InitStructure;
	// No StructInit call in API
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init (&NVIC_InitStructure);

    GPIO_InitTypeDef  GPIO_InitStructureAdc;
	GPIO_StructInit (&GPIO_InitStructureAdc);
    GPIO_InitStructureAdc.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStructureAdc.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructureAdc);

    ADC_DeInit(ADC1);
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1 , &ADC_InitStructure);

	// Configure ADC_IN6
	ADC_RegularChannelConfig(ADC1 , ADC_Channel_2 , 1, ADC_SampleTime_55Cycles5);
	// Enable ADC
	ADC_ITConfig(ADC1 , ADC_IT_EOC , ENABLE);
	ADC_ExternalTrigConvCmd(ADC1 , ENABLE);
	ADC_Cmd(ADC1 , ENABLE);

	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1));
	// Start ADC1 calibration
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));
}

static int a, b;

static uint16_t adc_current_sample;

void ADC1_IRQHandler(void)
{
	//portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
		input_buffer[adc_current_sample] = ADC_GetConversionValue(ADC1) << 3;
		adc_current_sample = (++adc_current_sample) & (AUDIOBUFSIZE - 1);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	};

	//portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

uint16_t get_current_sample(){
	return adc_current_sample;
}
