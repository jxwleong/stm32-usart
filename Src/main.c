
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "RCC.h"
#include "GPIO.h"
#include "USART.h"
#include "Timer.h"
#include "NVIC.h"
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static int i = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
char wordsBuffer[] = {
	'H',
	'e',
	'l',
	'l',
	'o',
	' ',
	'W',
	'o',
	'r',
	'l',
	'd',
	'!',
	'\n',
};

char commandBuffer[100];


void UARTtransmitBuffer(int i){
	  if(usartIsTxRegEmpty(uart5)){
		  (uart5)->DR = wordsBuffer[i];
		  i++;
		  if(wordsBuffer[i-1] == '\n'){
		 	i = 0;
		 }
	  }

}

void commandLineOperation(GPIORegs *port, GPIOPin pins, char *commandStr){
	if(!(strcasecmp(commandStr, "turn on"))){
		GPIOwritePins(port, pins, PIN_SET);
		disableTimer2Interrupt();
	}
	else if(!(strcasecmp(commandStr, "turn off"))){
		GPIOwritePins(port, pins, PIN_RESET);
		disableTimer2Interrupt();
	}
	else if(!(strcasecmp(commandStr, "blink"))){
		configureTimer2Interrupt();
	}
}

// The interrupt would be 125ms
void configureTimer2Interrupt(){
	RESET_TIMER_2_CLK_GATING();
	UNRESET_TIMER_2_CLK_GATING();
	ENABLE_TIMER_2_CLK_GATING();
	(timer2)->arr = 375000;
	(timer2)->psc = 29;
	(timer2)->cnt = 0;
	nvicEnableInterrupt(28);
	TIM_INTERRUPT_ENABLE(timer2, TIM_UIE);
	TIM_COUNTER_ENABLE(timer2);
}

void disableTimer2Interrupt(){

	nvicDisableInterrupt(28);
	TIM_COUNTER_DISABLE(timer2);
	TIM_INTERRUPT_DISABLE(timer2, TIM_UIE);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  RESET_UART5_CLK_GATING();
  UNRESET_UART5_CLK_GATING();
  ENABLE_UART5_CLK_GATING();
  ENABLE_GPIOC_CLK_GATING();
  ENABLE_GPIOD_CLK_GATING();
  ENABLE_GPIOG_CLK_GATING();
  //LED4
  GPIOConfigurePin(gpioG, GPIOPin14, GPIO_OUTPUT|GPIO_PUSH_PULL |GPIO_HI_SPEED|GPIO_NO_PULL);
  // UART5_TX
  GPIOConfigurePin(gpioC, GPIOPin12, GPIO_ALT_FUNC|GPIO_VERY_HI_SPEED);
  GPIOConfigureAltFunc(gpioC, GPIOPin12, AF8);

  // UART5_RX
  GPIOConfigurePin(gpioD, GPIOPin2, GPIO_ALT_FUNC|GPIO_VERY_HI_SPEED);
  GPIOConfigureAltFunc(gpioD, GPIOPin2, AF8);
  usartConfigure(uart5, USART_OVERSAMPLING_16 | USART_WORD_LENGTH_9BITS |	\
		  	  	  	  	USART_ENABLE | USART_STOP_BIT_1 | USART_PC_ENABLE | \
						USART_PARITY_ODD \
						| USART_TE_ENABLE | USART_RE_ENABLE ,115200, 45000000);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	 // UARTtransmitBuffer(i);

	  /*Read data*/

restart:
		  if(usartIsRxRegNotEmpty(uart5)){
			  commandBuffer[i] = (uart5)->DR;
			  if(usartIsTxRegEmpty(uart5)){
				  (uart5)->DR = commandBuffer[i];
			  }
			  if(commandBuffer[i] == '\b'){
				  // check for backspace
				  i--;
				  goto restart;
			  }
			  if(commandBuffer[i] == '\n'){
				  commandBuffer[i] = '\0';
				  commandLineOperation(gpioG, GPIOPin14, commandBuffer);
				  i = 0;
				  goto restart;
			  }
			  i++;

	  }
		  //i = 0;


  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 90;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
