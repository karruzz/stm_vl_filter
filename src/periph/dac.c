/*
 * dac.c
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */

#include "dac.h"

#include "misc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


// dac pin PA4
void sound_player_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	DMA_DeInit(DMA1_Channel3);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit (&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(DAC -> DHR12R1));
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_BufferSize = AUDIOBUFSIZE;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) output_buffer;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3 , &DMA_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructureDac;
	GPIO_StructInit (&GPIO_InitStructureDac);
    GPIO_InitStructureDac.GPIO_Pin =  GPIO_Pin_4;
    GPIO_InitStructureDac.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructureDac);

    // DAC channel1 Configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_StructInit (& DAC_InitStructure);
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T3_TRGO;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1 , &DAC_InitStructure);
}

void sound_player_start()
{
	// Enable everything
	DMA_Cmd(DMA1_Channel3 , ENABLE);
	DAC_Cmd(DAC_Channel_1 , ENABLE);
	DAC_DMACmd(DAC_Channel_1 , ENABLE);
}


