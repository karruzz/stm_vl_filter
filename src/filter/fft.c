/*
 * FFT.c
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */


#include "fft.h"

int SR[] = { -256, 0, 181, 236, 251, 254, 255 };
int SI[] = { 0, -256, -181, -97, -49, -25, -12};


extern unsigned int BitReverse(unsigned int x);
extern int Sqrt2(int x);

void FFTAdd(int x)
{
	ReSignal[BitReverse(FFTsample)] = x;
	ImSignal[FFTsample++] = 0;
}

void FFTCalculate()
{
	//FFTCompute();
	int i;
	int tr; int ti;
	int k;int ip;
	int le = 1;
	int le2; int ur; int ui;

	for (int j = 0; j < FFTORDER; j++)
	{
		le2 = le;	le <<= 1;
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

	FFTsample = 0;
}

void FFTComputeMagnitude()
{
	for (int j = 0; j < FFTSIZE>>1; j++)
	{
		Magnitude[j] = Sqrt2(Sqrt2(ReSignal[j]*ReSignal[j] + ImSignal[j]*ImSignal[j]));
	}
}
