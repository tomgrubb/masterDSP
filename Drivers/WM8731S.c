#include "WM8731S.h"

uint16_t rxDataBufferA[DMA_BUFF_SIZE];
uint16_t txDataBufferA[DMA_BUFF_SIZE];

void I2C_CODEC_Init(void)
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

void I2C_CODEC_Startup(void)
{
	int status = 1;

	status = status && I2C_CODEC_Write(POWER_DWN_CTRL, 0x0010);				// power up everything except the outputs

	status = status && I2C_CODEC_Write(LEFT_LINE_IN, 0x0017);					// enable left line in, 0dB attenuation
	status = status && I2C_CODEC_Write(RIGHT_LINE_IN, 0x0097);					// disable right line in, 0dB attenuation

	status = status && I2C_CODEC_Write(LEFT_PHONE_OUT, 0x0000);				// disable left phone out
	status = status && I2C_CODEC_Write(RIGHT_PHONE_OUT, 0x0000);				// disable right phone out

	status = status && I2C_CODEC_Write(ANALOG_CONTROL, 0x0013);				// DAC selected, mic muted, boost disabled
	status = status && I2C_CODEC_Write(DIGITAL_CONTROL, 0x0000);				// disable all DSP
	status = status && I2C_CODEC_Write(INTERFACE_FORMAT, CODEC_16bSlave);		// I2S format, 16 bits, default config
	status = status && I2C_CODEC_Write(SAMPLING_CTRL, 0x0002);					//

	status = status && I2C_CODEC_Write(ACTIVE_CONTROL, 0x0001);				// activate interface
	status = status && I2C_CODEC_Write(POWER_DWN_CTRL, 0x0000);				// enable outputs
}

int I2C_CODEC_Write(uint8_t reg, uint16_t data)
{
		unsigned long int TIMEOUT = 0;
		unsigned long int I2C_TIMEOUT_MAX = 20000;
		uint8_t status;

		uint8_t byte1 = ((reg << 1) & 0xfe) | ((data >> 8) & 0x01);
		uint8_t byte2 = data & 0xff;

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
		I2C_Send7bitAddress(I2C1, CODECADD, I2C_Direction_Transmitter);

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

		// send MSB
		I2C_SendData(I2C1, byte1);

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

		// send LSB
		I2C_SendData(I2C1, byte2);

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

void I2S_CODEC_GPIO_Config(void)
{
	// SCK --> PB10 (Bitclock)
	// WS --> PB12 (LRCLK
	// extSD --> PC2 (RX)
	// SD --> PC3 (TX)

	GPIO_InitTypeDef GPIO_InitStructure;

	// enable I2S peripheral GPIO clocks
	RCC_AHB1PeriphClockCmd((RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC) , ENABLE);

	// Configure GPIOs in Port B
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_10 | GPIO_Pin_12);
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// configure GPIOs in Port C
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3);
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// configure GPIOs in Port A
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Connect pins to I2S peripheral.
	GPIO_PinAFConfig(GPIOB, CODEC_I2S_WS_PINSRC, CODEC_I2S_GPIO_AF);
	GPIO_PinAFConfig(GPIOB, CODEC_I2S_SCK_PINSRC, CODEC_I2S_GPIO_AF);
	GPIO_PinAFConfig(GPIOC, CODEC_I2S_SDI_PINSRC, GPIO_AF_I2S2ext);
	GPIO_PinAFConfig(GPIOC, CODEC_I2S_SDO_PINSRC, CODEC_I2S_GPIO_AF);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, CODEC_I2S_GPIO_AF);		// master clock
}

void I2S_Module_Config(void)
{
	I2S_InitTypeDef I2SInitStruct;

	I2S_StructInit(&I2SInitStruct);

	SPI_I2S_DeInit(SPI2);
	I2S_Cmd(SPI2, DISABLE);

	RCC_I2SCLKConfig(RCC_I2SBus_APB1, RCC_I2SCLKSource_PLLI2S);

	RCC_PLLI2SCmd(ENABLE);									// enable I2S PLL clock
	while (RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET) {};

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	I2SInitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2SInitStruct.I2S_CPOL = I2S_CPOL_Low;
	I2SInitStruct.I2S_DataFormat = I2S_DataFormat_16b;
	I2SInitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2SInitStruct.I2S_Mode = I2S_Mode_MasterTx;
	I2SInitStruct.I2S_Standard = I2S_Standard_Phillips;

	I2S_Init(SPI2, &I2SInitStruct);

	//I2SInitStruct.I2S_Mode = I2S_Mode_MasterTx;
	I2S_FullDuplexConfig(I2S2ext, &I2SInitStruct);
}

void I2S_Audio_Start(void)
{
	int i;

	for (i = 0; i < DMA_BUFF_SIZE; i++)
	{
		rxDataBufferA[i] = 0xFFAA;
	}

	DMA_Cmd(I2S_TX_DMA_STREAM, ENABLE);
	DMA_Cmd(I2S_RX_DMA_STREAM, ENABLE);

	I2S_Cmd(I2S2ext, ENABLE);
	I2S_Cmd(SPI2, ENABLE);
}

void CODEC_DMA_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	DMA_InitTypeDef DMA_Init_TX;
	DMA_InitTypeDef DMA_Init_RX;

	// initialize DMA Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	// DMA setup for TX.
	DMA_StructInit(&DMA_Init_TX);	// set all configurations to default
	DMA_Cmd(I2S_TX_DMA_STREAM, DISABLE);
	DMA_DeInit(I2S_TX_DMA_STREAM);

	DMA_Init_TX.DMA_Channel = I2S_TX_DMA_CHANNEL;
	DMA_Init_TX.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	DMA_Init_TX.DMA_Memory0BaseAddr = (uint32_t)&txDataBufferA;
	DMA_Init_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init_TX.DMA_BufferSize = (uint32_t)DMA_BUFF_SIZE;
	DMA_Init_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	// 16 bit samples
	DMA_Init_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			// 16 bit samples
	DMA_Init_TX.DMA_Mode = DMA_Mode_Circular;
	DMA_Init_TX.DMA_Priority = DMA_Priority_High;
	DMA_Init_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Init_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(I2S_TX_DMA_STREAM, &DMA_Init_TX);

	// DMA setup for RX.
	DMA_Cmd(I2S_RX_DMA_STREAM, DISABLE);
	DMA_DeInit(I2S_RX_DMA_STREAM);

	DMA_Init_RX.DMA_Channel = I2S_RX_DMA_CHANNEL;
	DMA_Init_RX.DMA_PeripheralBaseAddr = I2S_RX_DMA_DREG;
	DMA_Init_RX.DMA_Memory0BaseAddr = (uint32_t)&rxDataBufferA;
	DMA_Init_RX.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_Init_RX.DMA_BufferSize = (uint32_t)DMA_BUFF_SIZE;
	DMA_Init_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	// 16 bit samples
	DMA_Init_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			// 16 bit samples
	DMA_Init_RX.DMA_Mode = DMA_Mode_Circular;
	DMA_Init_RX.DMA_Priority = DMA_Priority_High;
	DMA_Init_RX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init_RX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_Init_RX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(I2S_RX_DMA_STREAM, &DMA_Init_RX);

	DMA_ITConfig(I2S_RX_DMA_STREAM, DMA_IT_TC | DMA_IT_HT, ENABLE); // enable interrupts

	// Start DMA channels
	SPI_I2S_DMACmd(I2S_MODULE, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd(I2S_EXT, SPI_I2S_DMAReq_Rx, ENABLE);

	DMA_Cmd(I2S_TX_DMA_STREAM, DISABLE);
	DMA_Cmd(I2S_RX_DMA_STREAM, DISABLE);
}

void I2S_InterruptInit(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

    // configure interrupt
    NVIC_InitStruct.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void DMA1_Stream3_IRQHandler(void)
{
	int i;
	for (i = 0; i < DMA_BUFF_SIZE; i++)
	{
		rxDataBufferA[i] = 0xFFAA;
	}
}

