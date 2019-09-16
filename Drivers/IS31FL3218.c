/*
 * IS31FL3218.c
 *
 *	all drivers for I2C LED driver IC
 *
 *  Created on: 11Jul.,2019
 *      Author: Tom
 */

#include "IS31FL3218.h"

int LED_UpdateFLAG = 0;
int PWM_UpdateFLAG = 0;

void I2C_ledDriver_init(void)
{
		I2C_InitTypeDef I2C1_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

		// SCL -> PB6, SDA -> PB9

		//Enable the i2c
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

		// enable GPIO Port B
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		//Connect GPIO pins to peripheral
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1); // SCL
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // SDA

		// Configure GPIOs
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// reset for configuration
		I2C_DeInit(I2C1);

		// disable peripheral before initialisation
		I2C_Cmd(I2C1, DISABLE);

		// state I2C1 operational parameters
		I2C1_InitStructure.I2C_Mode = I2C_Mode_I2C;				// standard I2C mode
		I2C1_InitStructure.I2C_Ack = I2C_Ack_Disable;			// disable ACKs
		I2C1_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C1_InitStructure.I2C_ClockSpeed = 25000;				// Clock = 100 kHz
		I2C1_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		// I2C_AcknowledgedAddress_7bit
		I2C1_InitStructure.I2C_OwnAddress1 = 0xEE;				// not required

		I2C_Init(I2C1, &I2C1_InitStructure);					// initialize module

		I2C_Cmd(I2C1, ENABLE);	// enable module
}



// writes sequentially to IS31FL3218 registers
//
// PARAMS:
// reg - start register address
// data - pointer to data structure for PWM values
// size - number of registers to be written

int I2C_ledDriver_SeqWrite(uint8_t reg, uint8_t *data, int size)
{
	unsigned long int TIMEOUT;
	unsigned long int I2C_TIMEOUT_MAX = 20000;
	uint8_t status;
	int i;
	uint8_t byte;

	TIMEOUT = I2C_TIMEOUT_MAX;

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
	{
		TIMEOUT--;
		if (TIMEOUT <= 0)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			return 0;
		}
	}

	// generate a START condition
	I2C_GenerateSTART(I2C1, ENABLE);
	TIMEOUT = I2C_TIMEOUT_MAX;

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET)
	{
		TIMEOUT--;
		if (TIMEOUT <= 0)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			return 0;
		}
	}
	status = I2C1->SR2;
	status = I2C1->SR1;
	TIMEOUT = I2C_TIMEOUT_MAX;

	// send I2C device Address and clear ADDR
	I2C_Send7bitAddress(I2C1, LED_DRVADD, I2C_Direction_Transmitter);

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
	{
		TIMEOUT--;
		if (TIMEOUT <= 0)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			return 0;
		}
	};

	status = I2C1->SR2;
	status = I2C1->SR1;
	TIMEOUT = I2C_TIMEOUT_MAX;

	// send register byte
	I2C_SendData(I2C1, reg);

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
	{
		TIMEOUT--;
		if (TIMEOUT <= 0)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			return 0;
		}
	};

	status = I2C1->SR2;
	status = I2C1->SR1;
	TIMEOUT = I2C_TIMEOUT_MAX;
	i = 0;	// reset index

	while ((size--) >= 0)
	{
		// pass value to
		byte = data[i];

		// send data byte
		I2C_SendData(I2C1, byte);

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
		{

			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		};

		status = I2C1->SR2;
		status = I2C1->SR1;
		TIMEOUT = I2C_TIMEOUT_MAX;

	}
	I2C_GenerateSTOP(I2C1, ENABLE);
	return 1;
}

int I2C_ledDriver_Write(uint8_t reg, uint8_t data)
{
		unsigned long int TIMEOUT = 0;
		unsigned long int I2C_TIMEOUT_MAX = 20000;
		uint8_t status;

		TIMEOUT = I2C_TIMEOUT_MAX;

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
		{
			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		}

		// generate a START condition
		I2C_GenerateSTART(I2C1, ENABLE);
		TIMEOUT = I2C_TIMEOUT_MAX;

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET)
		{
			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		}
		status = I2C1->SR2;
		status = I2C1->SR1;
		TIMEOUT = I2C_TIMEOUT_MAX;

		// send I2C device Address and clear ADDR
		I2C_Send7bitAddress(I2C1, LED_DRVADD, I2C_Direction_Transmitter);

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
		{
			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		};

		status = I2C1->SR2;
		status = I2C1->SR1;
		TIMEOUT = I2C_TIMEOUT_MAX;

		// send register byte
		I2C_SendData(I2C1, reg);

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
		{
			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		};

		status = I2C1->SR2;
		status = I2C1->SR1;
		TIMEOUT = I2C_TIMEOUT_MAX;

		// send data byte
		I2C_SendData(I2C1, data);

		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
		{
			TIMEOUT--;
			if (TIMEOUT <= 0)
			{
				I2C_GenerateSTOP(I2C1, ENABLE);
				return 0;
			}
		};

		I2C_GenerateSTOP(I2C1, ENABLE);
		return 1;
}

void ledDrv_update(void)
{
	I2C_ledDriver_Write(updateReg, 0x00);
}

void ledDrv_shutdown(int state)
{
	I2C_ledDriver_Write(shutdwnReg, state);
}

void ledDrv_reset(void)
{
	I2C_ledDriver_Write(resetReg, 0);
	I2C_ledDriver_Write(shutdwnReg, 0);
	I2C_ledDriver_Write(shutdwnReg, 1);
}

int ledDrv_PWMinit(int initValue)
{
	char pwmData[16];

	for (int n = 0; n < 16; n++)
	{
		pwmData[n] = initValue;
	}

	I2C_ledDriver_SeqWrite(pwmReg1, &pwmData, 16);

	ledDrv_update();

	return 1;
}

void setupLedDriver(void)
{
	// configure I2C1 for LED driver
	I2C_ledDriver_init();

	// pull out of shutdown state
	ledDrv_reset();

	// init all PWM values
	ledDrv_PWMinit(0xFF);
	I2C_ledDriver_Write(pwmReg1, 0xFF);

	// enable LEDs
	I2C_ledDriver_Write(ctrlReg1, 1);
	I2C_ledDriver_Write(ctrlReg2, 0);
	I2C_ledDriver_Write(ctrlReg3, 0);

	// force update
	I2C_ledDriver_Write(updateReg, 0x00);
}

void setLEDUpdateFlag(int state)
{
	LED_UpdateFLAG = state;
}

int getLEDUpdateFlag(void)
{
	return LED_UpdateFLAG;
}

void setPWMUpdateFlag(int state)
{
	PWM_UpdateFLAG = state;
}

int getPWMUpdateFlag(void)
{
	return PWM_UpdateFLAG;
}

int stuffCtrlRegValues(int reg, int *value)
{
	int i;
	int regValue = 0;

	if (reg == ctrlReg1)
	{
		for (i = 0; i < 7; i++)
		{
			regValue |= value[i];
		}
		ctrlReg1Data = regValue;
	}
	else if (reg == ctrlReg2)
	{
		for (i = 8; i < 15; i++)
		{
			regValue |= value[i];
		}
		ctrlReg2Data = regValue;
	}
	else if (reg == ctrlReg3)
	{
		for (i = 16; i < MAX_LEDS; i++)
		{
			regValue |= value[i];
		}
		ctrlReg3Data = regValue;
	}
	else
	{
		return 0;
	}
	return 1;
}

int * packCtrlRegData(void)
{
	int packedData[3];

	packedData[0] = ctrlReg1Data;
	packedData[1] = ctrlReg2Data;
	packedData[2] = ctrlReg3Data;

	return packedData;
}
