#ifndef MAIN_H_
#define MAIN_H_

#include "IS31FL3218.h"

#define MAX_LEDS 13
#define MAX_SW 2

static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

volatile int swState[MAX_SW];
volatile int ledState[MAX_LEDS];
volatile int ledPWM[MAX_LEDS];

int block = 0;

#endif /* MAIN_H_ */
