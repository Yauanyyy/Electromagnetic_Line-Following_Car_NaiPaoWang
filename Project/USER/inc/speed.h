#ifndef __SPEED_H
#define __SPEED_H

#include "headfile.h"


typedef enum
{
	LEFT = 0,
	RIGHT
} deviceSide;

void putOut (deviceSide LoR, float speed);
void motorPWMOut (deviceSide LoR, int16 pwm);

#endif

int Z_change (float data1);
