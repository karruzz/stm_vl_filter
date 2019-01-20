/*
 * LowPass.c
 *
 *  Created on: May 18, 2015
 *      Author: karruzz
 *     License: GNU GPL 3
 */
#include "low_pass.h"

/*
static const int A0 = 260;
static const int A1 = 260;
static const int B0 = -503;

static int _y0, _y1, _x1;

int FilterLP(int x0)
{
	_y0 = (A0 * _x1 + A1 * x0 - B0 * _y1) >> 10;
	_y1 = _y0;
	_x1 = x0;
	return _y0;
}

*/

#define NZEROS 2
#define NPOLES 2
#define GAIN   12
static int xv[NZEROS+1], yv[NPOLES+1];

int FilterLP(int x)
{
	xv[0] = xv[1]; xv[1] = xv[2];
	xv[2] = x / GAIN;
	yv[0] = yv[1]; yv[1] = yv[2];
	yv[2] = (1024 * (xv[0] + xv[2]) + 2048 * xv[1] + (-376 * yv[0]) + (1058 * yv[1])) >> 10;
	return yv[2];
}
