/*
 * FFT.c
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */


#include "FFT.h"

unsigned int sample = 0;

int SR[] = { -256, 0, 181, 236, 251, 254, 255 };
int SI[] = { 0, -256, -181, -97, -49, -25, -12};

static inline unsigned int BitReverse(unsigned int x)
{
	unsigned int y=0;
	for(int position = FFTORDER-1; position >= 0; position--)
	{
		y+=((x&1)<<position);
		x >>= 1;
	}
	return y;
}

void FFTAdd(int x)
{
	ReSignal[BitReverse(sample++)] = x;
	ImSignal[sample] = 0;
	if (sample != FFTSIZE) return;
	sample = 0;
	FFTIsFull = TRUE;
	FFTIsMagnitudeReady = FALSE;
}

static inline int sqrt2(int x)
{
	int y = 0;
	int x0 = 200;
	for (int i = 0; i < 8; i++)
	{
		y = (x0 + x/x0) / 2;
		x0 = y;
	}

	return y;
}

void FFTCalculate()
{
	int i;
	int tr; int ti;
	int k;int ip;
	int le = 1;
	int le2; int ur; int ui;

	for (int j = 0; j < FFTORDER; j++)
	{
		le2 = le;	le *= 2;
		ur = 256; ui = 0;
		for (k = 0; k < le2; k++)
		{
			for (i = k; i < FFTSIZE; i+=le)
			{
				ip = i + le2;
				tr = (ReSignal[ip] * ur - ImSignal[ip] * ui) >> 8;
				ti = (ReSignal[ip] * ui + ImSignal[ip] * ur) >> 8;

				ReSignal[ip] = ReSignal[i] - tr;
				ImSignal[ip] = ImSignal[i] - ti;

				ReSignal[i] = ReSignal[i] + tr;
				ImSignal[i] = ImSignal[i] + ti;
			}
			tr = ur;
			ur = (tr * SR[j] - ui * SI[j])>>8;
			ui = (tr * SI[j] + ui * SR[j])>>8;
		}
	}

	for (int j = 0; j < FFTSIZE>>1; j++)
	{
		Magnitude[j] = sqrt2(ReSignal[j]*ReSignal[j] + ImSignal[j]*ImSignal[j]) >> 3;
	}

	FFTIsFull = FALSE;
}
