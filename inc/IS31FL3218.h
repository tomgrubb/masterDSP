/*
 * IS31FL3218.h
 *
 *  Created on: 11Jul.,2019
 *      Author: Tom
 */

#ifndef DRIVERS_IS31FL3218_H_
#define DRIVERS_IS31FL3218_H_

#include "stm32f4xx_i2c.h"

#define LED_DRVADD 0xA8				// slave address of IS31FL3218
#define shutdwnReg (char)0x00		// address of shutdown register

#define pwmReg1 (char)0x01			// PWM register for LED 1
#define pwmReg2 (char)0x02			// PWM register for LED 2
#define pwmReg3 (char)0x03			// PWM register for LED 3
#define pwmReg4 (char)0x04			// PWM register for LED 4
#define pwmReg5 (char)0x05			// PWM register for LED 5
#define pwmReg6 (char)0x06			// PWM register for LED 6
#define pwmReg7 (char)0x07			// PWM register for LED 7
#define pwmReg8 (char)0x08			// PWM register for LED 8
#define pwmReg9 (char)0x09			// PWM register for LED 9
#define pwmReg10 (char)0x0A			// PWM register for LED 10
#define pwmReg11 (char)0x0B			// PWM register for LED 11
#define pwmReg12 (char)0x0C			// PWM register for LED 12
#define pwmReg13 (char)0x0D			// PWM register for LED 13
#define pwmReg14 (char)0x0E			// PWM register for LED 14
#define pwmReg15 (char)0x0F			// PWM register for LED 15
#define pwmReg16 (char)0x10			// PWM register for LED 16
#define pwmReg17 (char)0x11			// PWM register for LED 17
#define pwmReg18 (char)0x12			// PWM register for LED 18

#define ctrlReg1 (char)0x13			// enable for LEDs 1-6
#define ctrlReg2 (char)0x14			// enable for LEDs 7-12
#define ctrlReg3 (char)0x15			// enable for LEDs 13-18

#define updateReg (char)0x16		// update states
#define resetReg (char)0x17			// reset all registers to default

#define MAX_PWM (char)0xFF			// maximum PWM value
#define MAX_LEDS 13

void setLEDUpdateFlag(int state);
int getLEDUpdateFlag(void);
void setPWMUpdateFlag(int state);
int getPWMUpdateFlag(void);

extern volatile int ctrlReg1Data;
extern volatile int ctrlReg2Data;
extern volatile int ctrlReg3Data;

#endif /* DRIVERS_IS31FL3218_H_ */
