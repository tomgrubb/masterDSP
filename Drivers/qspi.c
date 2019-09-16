#include "stm32f4xx.h"

void InitQSPI_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// enable QSPI peripheral clock
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_QSPI,ENABLE);

	// enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// map GPIOs to Alternative Functions
	// PC11 -> #CS
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF9_QUADSPI);
	// PB1 -> CLK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF9_QUADSPI);
	// PA6 -> D0
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF10_QUADSPI);
	// PA7 -> D1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF10_QUADSPI);
	// PC4 -> D2
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF10_QUADSPI);
	// PC5 -> D3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF10_QUADSPI);

	// Configure GPIOs (PORTA)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure GPIOs (PORTB)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure GPIOs (PORTC)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ConfigureQSPI(void)
{
	QSPI_InitTypeDef QSPI_InitStruct;
	QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStruct;

	// configure QSPI structure for init
	QSPI_StructInit(&QSPI_InitStruct);					// init to default values
	QSPI_InitStruct.QSPI_CKMode = QSPI_CKMode_Mode0;	// Clock Mode 0 selected
	QSPI_InitStruct.QSPI_CSHTime = 0x07;				// CS high time is 8 cycles between instructions
	QSPI_InitStruct.QSPI_DFlash = QSPI_DFlash_Disable; 	// Dual Flash mode disabled
	QSPI_InitStruct.QSPI_FSelect = 0x00;				// Flash 1 selected
	QSPI_InitStruct.QSPI_FSize = 0x09;					// Flash size is 2^(9+1) = 1024 bytes
	QSPI_InitStruct.QSPI_Prescaler = 0xFF;				// QSPI Clock is Fsck/255
	QSPI_InitStruct.QSPI_SShift = QSPI_SShift_NoShift;	// sampled on the rising edge of clock
	QSPI_Init(&QSPI_InitStruct);						// init module

	// FIFO threshold is one byte
	QSPI_SetFIFOThreshold(0x00001);

	// configure QSPI comms structure
	QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStruct);		// init to default values
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_DummyCycles = 0;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_NoData;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_DHHC        = QSPI_ComConfig_DHHC_Disable;
	QSPI_ComConfig_InitStruct.QSPI_ComConfig_DDRMode     = QSPI_ComConfig_DDRMode_Disable;

	QSPI_Cmd(ENABLE);	// ENABLE QUAD SPI
}
