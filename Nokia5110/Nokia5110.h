/*
 * Nokia5110.h
 *
 *  Created on: May 14, 2015
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

#ifndef NOKIA5110_NOKIA5110_H_
#define NOKIA5110_NOKIA5110_H_

#include "stm32f10x.h"

#define MAX_X  84
#define MAX_Y  48
#define MAX_Y_OCTETS  (MAX_Y >> 3)

#define CONTRAST  0xB3

static const char ASCII[][5] = {
		   {0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
		  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
		  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
		  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
		  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
		  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
		  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
		  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
		  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
		  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
		  ,{0x00, 0x00, 0x00, 0x00, 0x00} // 40 ' '
};

void Nokia5110Init(void);
void Nokia5110Clear(void);

// 48x84 bitmap image
void Nokia5110DrawImage(const char *ptr);
void Nokia5110DrawHist(const uint8_t *points);

void Nokia5110OutChar(unsigned char data);
void Nokia5110OutString(char *ptr);
// column = [0; 11], raw = [0; 5]
void Nokia5110SetCursor(unsigned char column, unsigned char raw);

#endif /* NOKIA5110_NOKIA5110_H_ */
