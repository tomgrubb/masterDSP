/*
 * WM8731S.h
 *
 *  Created on: 4Aug.,2019
 *      Author: Tom
 */

#ifndef DRIVERS_WM8731S_H_
#define DRIVERS_WM8731S_H_

#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"

#define ADD_CSB0			(uint8_t)0x1A
#define ADD_CSB1			(uint8_t)0x1B
#define CODECADD			(ADD_CSB0 << 1)		// slave address of WM8731S CODEC

#define LEFT_LINE_IN		(uint8_t)0x00
#define RIGHT_LINE_IN		(uint8_t)0x01
#define LEFT_PHONE_OUT		(uint8_t)0x02
#define RIGHT_PHONE_OUT		(uint8_t)0x03
#define ANALOG_CONTROL		(uint8_t)0x04
#define DIGITAL_CONTROL		(uint8_t)0x05
#define POWER_DWN_CTRL		(uint8_t)0x06
#define INTERFACE_FORMAT	(uint8_t)0x07
#define SAMPLING_CTRL		(uint8_t)0x08
#define ACTIVE_CONTROL		(uint8_t)0x09
#define RESET_CONTROL		(uint8_t)0x0F

#define I2S_MODULE                      SPI2
#define I2S_EXT                  		I2S2ext
#define I2S_CLK               			RCC_APB1Periph_SPI2
#define CODEC_I2S_ADDRESS              	0x4000380C
#define CODEC_I2S_EXT_ADDRESS          	0x4000340C
#define CODEC_I2S_GPIO_AF              	GPIO_AF_SPI2
#define CODEC_I2S_IRQ                  	SPI2_IRQn
#define CODEC_I2S_EXT_IRQ              	SPI2_IRQn

#define CODEC_I2S_WS_PIN               	GPIO_Pin_12
#define CODEC_I2S_SCK_PIN              	GPIO_Pin_10
#define CODEC_I2S_SDI_PIN              	GPIO_Pin_2
#define CODEC_I2S_SDO_PIN              	GPIO_Pin_3

#define CODEC_I2S_WS_PINSRC            	GPIO_PinSource12
#define CODEC_I2S_SCK_PINSRC           	GPIO_PinSource10
#define CODEC_I2S_SDI_PINSRC          	GPIO_PinSource2
#define CODEC_I2S_SDO_PINSRC           	GPIO_PinSource3
#define I2S_IRQHandler				   	SPI2_IRQHandler

#define DMA_BUFF_SIZE					0xFF
#define DMA_PERIPH_DATA_SIZE     		DMA_PeripheralDataSize_HalfWord
#define DMA_MEM_DATA_SIZE        		DMA_MemoryDataSize_HalfWord
#define I2S_DMA_CLOCK            		RCC_AHB1Periph_DMA1
#define I2S_TX_DMA_STREAM        		DMA1_Stream4
#define I2S_TX_DMA_DREG             	CODEC_I2S_ADDRESS
#define I2S_TX_DMA_CHANNEL       		DMA_Channel_0
#define I2S_TX_DMA_IRQ              	DMA1_Stream4_IRQn
#define I2S_TX_DMA_FLAG_TC          	DMA_FLAG_TCIF4
#define I2S_TX_DMA_FLAG_HT          	DMA_FLAG_HTIF4
#define I2S_TX_DMA_FLAG_FE          	DMA_FLAG_FEIF4
#define I2S_TX_DMA_FLAG_TE          	DMA_FLAG_TEIF4
#define I2S_TX_DMA_FLAG_DME         	DMA_FLAG_DMEIF4
#define I2S_RX_DMA_STREAM		 		DMA1_Stream3
#define I2S_RX_DMA_DREG         		CODEC_I2S_EXT_ADDRESS
#define I2S_RX_DMA_CHANNEL      		DMA_Channel_3
#define I2S_RX_DMA_IRQ          		DMA1_Stream3_IRQn
#define I2S_RX_DMA_FLAG_TC      		DMA_FLAG_TCIF3
#define I2S_RX_DMA_FLAG_HT     	 		DMA_FLAG_HTIF3
#define I2S_RX_DMA_FLAG_FE      		DMA_FLAG_FEIF3

#define I2S_RX_DMA_FLAG_TE      		DMA_FLAG_TEIF3
#define I2S_RX_DMA_FLAG_DME     		DMA_FLAG_DMEIF3
#define I2S_RX_DMA_REG          		DMA1
#define I2S_RX_DMA_ISR          		LISR
#define I2S_RX_DMA_IFCR         		LIFCR

uint16_t CODEC_16bMaster = 0x0042;
uint16_t CODEC_16bSlave = 0x0002;

int I2C_CODEC_Write(uint8_t reg, uint16_t data);
void I2C_CODEC_Init(void);
void I2C_CODEC_Startup(void);
void CODEC_DMA_Config(void);
void I2S_CODEC_GPIO_Config(void);
void I2C_CODEC_Init(void);
void I2C_CODEC_Startup(void);
void I2S_Module_Config(void);

#endif /* DRIVERS_WM8731S_H_ */
