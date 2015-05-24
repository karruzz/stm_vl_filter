/*
 * FFT.c
 *
 *  Created on: May 16, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */


#include "FFT.h"


const int factor = 256;

//int SR[] = { int(-1 * k), 0, 0.5 * k, 0.7071 * k, 0.809 * k, 0.866 * k };
int SR[] = { -256, 0, 181, 236, 251, 254, 255 };
//float SR[] = { -1, 0, 0.7071, 0.9239, 0.9808, 0.9952, 0.9988 };
// -sin(PI/index)
//int SI[] = { 0, 1 * k, -0.866 * k, -0.7071 * k, -0.5877 * k, -0.5 * k};
int SI[] = { 0, -256, -181, -97, -49, -25, -12};
//float SI[] = { 0, -1, -0.7071, -0.3827, -0.1951, -0.098, -0.0491};

static inline unsigned int Reverse(unsigned int x)
{
	unsigned int y=0;
	for(int position = M-1; position >= 0; position--)
	{
		y+=((x&1)<<position);
		x >>= 1;
	}
	return y;
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
	int j;
	int i;
	char temp;
	int reversed;

	for (i = 1; i < FFTSIZE - 1; i++)
	{
		temp = ReSignal[i];
		reversed = Reverse(i);
		if (reversed <= i) continue;
		ReSignal[i] = ReSignal[reversed];
		ReSignal[reversed] = temp;
	}

	int tr; int ti;
	int k;int ip;
	int le = 1;
	int le2; int ur; int ui;
	for (j = 0; j < M; j++)
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

	for (j = 0; j < FFTSIZE; j++)
	{
		Magnitude[j] = sqrt2(ReSignal[j]*ReSignal[j] + ImSignal[j]*ImSignal[j]);
	}
}
