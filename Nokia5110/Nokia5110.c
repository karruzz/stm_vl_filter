/*
 * Nokia5110.c
 *
 *  Created on: May 16, 2015
 */

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

//      SPI1 SPI2
// SS   PA4  PB12 Alternate function push-pull (50MHz)
// SCK  PA5  PB13 Alternate function push-pull (50MHz)
// MISO PA6  PB14 Input pull-up
// MOSI PA7  PB15 Alternate function push-pull (50MHz)

#include "Nokia5110.h"

enum spiSpeed { SPI_SLOW , SPI_MEDIUM , SPI_FAST };

static const uint16_t speeds [] = {
	[SPI_SLOW] = SPI_BaudRatePrescaler_64 ,
	[SPI_MEDIUM] = SPI_BaudRatePrescaler_8 ,
	[SPI_FAST] = SPI_BaudRatePrescaler_2 };

#define SPI        SPI2
#define SPI_SPEED  SPI_MEDIUM

#define RESET_PIN  GPIO_Pin_11
#define DC_PIN     GPIO_Pin_10
#define SOFT_SS    GPIO_Pin_12

enum writeMode { COMMAND, DATA };

static void spiInit(SPI_TypeDef *SPIx)
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit (& SPI_InitStructure);

	if (SPIx == SPI2)
	{
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	    GPIO_InitTypeDef  GPIO_InitStructureSPI;
		GPIO_StructInit (&GPIO_InitStructureSPI);
	    GPIO_InitStructureSPI.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	    GPIO_InitStructureSPI.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructureSPI.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOB, &GPIO_InitStructureSPI);

	    GPIO_InitTypeDef  GPIO_InitStructureOUT;
		GPIO_StructInit (&GPIO_InitStructureOUT);
	    GPIO_InitStructureOUT.GPIO_Pin = GPIO_Pin_14 ;
	    GPIO_InitStructureOUT.GPIO_Mode = GPIO_Mode_IPU;
	    GPIO_InitStructureOUT.GPIO_Speed = GPIO_Speed_2MHz;
	    GPIO_Init(GPIOB, &GPIO_InitStructureOUT);

	    GPIO_InitStructureOUT.GPIO_Pin = RESET_PIN | DC_PIN | SOFT_SS;
	    GPIO_InitStructureOUT.GPIO_Mode = GPIO_Mode_Out_PP;
	    GPIO_InitStructureOUT.GPIO_Mode = GPIO_Mode_Out_PP;
	    GPIO_InitStructureOUT.GPIO_Speed = GPIO_Speed_10MHz;
	    GPIO_Init(GPIOB, &GPIO_InitStructureOUT);
	}
	else return;

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SPEED];
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx , &SPI_InitStructure);
	SPI_Cmd(SPIx , ENABLE);
	SPIx ->CR1 = (SPIx ->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[SPI_SPEED];
}

static void delay(int nops)
{
    while(nops > 0)
    {
        asm ("nop");
        nops--;
    }
}

static void lcdwrite(enum writeMode mode, char message)
{
	  GPIO_WriteBit(GPIOB, SOFT_SS, Bit_RESET);
	  if(mode == COMMAND) {
		GPIO_WriteBit(GPIOB, DC_PIN, Bit_RESET);
	    SPI_I2S_SendData(SPI , message);
	  }
	  else {
		GPIO_WriteBit(GPIOB, DC_PIN, Bit_SET);
	    SPI_I2S_SendData(SPI , message);
	  }

	  while (SPI_I2S_GetFlagStatus(SPI , SPI_I2S_FLAG_RXNE) == RESET);
	  SPI_I2S_ReceiveData(SPI);

	  GPIO_WriteBit(GPIOB, SOFT_SS, Bit_SET);
}

static void fastlcdwrite(char message)
{
	SPI_I2S_SendData(SPI , message);
	while (SPI_I2S_GetFlagStatus(SPI , SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI);
}

void Nokia5110Init(void)
{
	spiInit(SPI);

	GPIO_WriteBit(GPIOB, DC_PIN, Bit_SET);
	GPIO_WriteBit(GPIOB, SOFT_SS, Bit_SET);

	GPIO_WriteBit(GPIOB, RESET_PIN, Bit_RESET); // x is Bit_SET or Bit_RESET
	delay(5000);
	GPIO_WriteBit(GPIOB, RESET_PIN, Bit_SET);

	lcdwrite(COMMAND, 0x21);              // chip active; vertical addressing mode (V = 1); use extended instruction set (H = 1)
										  // set LCD Vop (contrast), which may require some tweaking:
	lcdwrite(COMMAND, CONTRAST);          // try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
	lcdwrite(COMMAND, 0x04);              // set temp coefficient
	lcdwrite(COMMAND, 0x14);              // LCD bias mode 1:48: try 0x13 or 0x14

	lcdwrite(COMMAND, 0x20);              // we must send 0x20 before modifying the display control mode
	lcdwrite(COMMAND, 0x0C);              // set display control to normal mode: 0x0D for inverse
	delay(100);
}

void Nokia5110OutChar(unsigned char data){
  int i;
  for(i=0; i<5; i=i+1){
    lcdwrite(DATA, ASCII[data - 0x30][i]);
  }
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
}

void Nokia5110OutString(char *ptr){
  while(*ptr){
    Nokia5110OutChar((unsigned char)*ptr);
    ptr = ptr + 1;
  }
}

void Nokia5110SetCursor(unsigned char column, unsigned char raw){
  if((column > 11) || (raw > 5)) return;

  // multiply newX by 7 because each character is 7 columns wide
  lcdwrite(COMMAND, 0x80|(column*7));     // setting bit 7 updates X-position
  lcdwrite(COMMAND, 0x40|raw);         // setting bit 6 updates Y-position
}

void Nokia5110Clear(void)
{
  int i;
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, 0x00);
  }
  Nokia5110SetCursor(0, 0);
}

void Nokia5110DrawHist(const char *points)
{
  int i;
  Nokia5110SetCursor(0, 0);

  GPIO_WriteBit(GPIOB, SOFT_SS, Bit_RESET);
  GPIO_WriteBit(GPIOB, DC_PIN, Bit_SET);

  for(i = 0; i < MAX_Y_OCTETS * MAX_X; i++)
	  fastlcdwrite(points[i]);

  GPIO_WriteBit(GPIOB, SOFT_SS, Bit_SET);
}

void Nokia5110DrawImage(const char *ptr)
{
  int i;
  Nokia5110SetCursor(0, 0);
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, ptr[i]);
  }
}

