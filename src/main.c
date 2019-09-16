/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"

//void DMA1_Stream3_IRQHandler(void)
//{
//	int k = 0;
//	k++;
//
//	// see if we get here
//}

volatile int ctrlReg1Data = 0;
volatile int ctrlReg2Data = 0;
volatile int ctrlReg3Data = 0;

extern uint16_t rxDataBufferA[0xFF];
extern uint16_t txDataBufferA[0xFF];

void SPI2_IRQHandler(void)
{
    /* Make sure that interrupt flag is set */
    if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != RESET)
    {
    	I2S2ext->DR = 0xAA03;
    	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
    }
}

void EXTI15_10_IRQHandler(void)
{
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
    	swState[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
    	TIM_Cmd(TIM2, ENABLE);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}

void TIM2_IRQHandler(void)
{
    /* Make sure that interrupt flag is set */
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
    	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == swState[0])
    	{
    		GPIO_ToggleBits(GPIOB, GPIO_Pin_7);
    	}
    	TIM_Cmd(TIM2, DISABLE);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}


// update LEDs every time this timer expires
void TIM3_IRQHandler(void)
{
	int * ctrlRegData[3];

    /* Make sure that interrupt flag is set */
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
    	// check LED value change
    	if (getLEDUpdateFlag())
    	{
    		stuffCtrlRegValues(ctrlReg1, &ledState);
    		stuffCtrlRegValues(ctrlReg2, &ledState);
    		stuffCtrlRegValues(ctrlReg3, &ledState);
    		ctrlRegData[0] = ctrlReg1Data;
    		ctrlRegData[1] = ctrlReg2Data;
    		ctrlRegData[2] = ctrlReg3Data;
    		I2C_ledDriver_SeqWrite(ctrlReg1, &ctrlRegData, 3);
    	}
    	// check PWM state change
    	if (getPWMUpdateFlag())
    	{
    		I2C_ledDriver_SeqWrite(pwmReg1, &ledPWM, MAX_LEDS);
    	}
    	ledDrv_update();	// push new states

    	block = 0;

    	TIM_Cmd(TIM3, ENABLE);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

int sysInit(void)
{
	//	InitQSPI_GPIO();
	//	ConfigureQSPI();
		setupLD2();

		I2C_CODEC_Init();

		CODEC_DMA_Config();
		I2S_CODEC_GPIO_Config();
		I2S_Module_Config();
		I2C_CODEC_Startup();
		//I2S_InterruptInit();
		I2S_Audio_Start();

		//setupLedDriver();

		//setupTimer2();
		//setupTimer3();
		//setupEXTI();

		return 1;
}

void binaryClockLED(int bit, int state)
{
	state++;
	if (state > MAX_LEDS)
	{
		state = 0;
	}
	for (int i = 0; i < MAX_LEDS; i++)
	{
		bit = (state & (0x01 << i));
		ledState[i] = bit;
	}
	setLEDUpdateFlag(1);
}

int main(void)
{
	int STARTUP_STATUS;
	int state = 0;
	int bit = 0;

	int LR = 0;

	uint16_t rxData = 0;
	uint16_t txData = 0;

	STARTUP_STATUS = sysInit();

	int i;

	for (i = 0; i < 0xFF; i++)
	{
		txData = rxDataBufferA[i];
		SPI_I2S_SendData(SPI2, txData);
	}

	while (1)
	{
//	    if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != RESET)
//	    {
//	    	rxData = SPI_I2S_ReceiveData(I2S2ext);
//	    	if (rxData > 0)
//	    	{
//	    		GPIO_SetBits(GPIOB, GPIO_Pin_7);
//	    	}
//	    	else
//	    	{
//	    		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
//	    	}
//	    	SPI_I2S_SendData(SPI2, rxData);
//
//	    	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	    }
//
//	    if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) != RESET)
//	    {
//	    	rxData = SPI_I2S_ReceiveData(I2S2ext);
//
//	    	SPI_I2S_ClearFlag(I2S2ext, SPI_I2S_FLAG_RXNE);
//	    }
//		while (block)
//		{
//			// do nothing
//		}
//		block = 1;
//		if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
//		{
//			SPI_I2S_SendData(SPI2, 0xFA07);
//
//			TIM_Cmd(TIM3, ENABLE);
//		}
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
