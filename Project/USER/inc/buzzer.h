#ifndef __BUZZER_H
#define __BUZZER_H

#include "headfile.h"

void buzzer_init();

void buzzer_beep(uint8 times);
	
void buzzer_on();
void buzzer_off();

#endif
