#include "main.h"
#include <stdio.h>

#define ARR_VALUE 20000
#define DEGREES_90 ARR_VALUE / 20 * 1.28
#define DEGREES_0 ARR_VALUE / 20 * 1.5
#define DEGREES_MINUS_90 ARR_VALUE / 20 * 1.72

volatile int timespan = 0;                              // Total pulse width
volatile int lastcounter = 0;                           // Timer counter value of the last event
volatile int newcounter = 0;                            // Timer counter value of the current event
volatile int overflow = 0;                              // Count the number of overflows

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

static void pinSetup(void);
static void timerSetup(void);
static void setDutyCycleChannel1(float compareValue);
static void setDutyCycleChannel2(float compareValue);
static void rotateVehicle(int rotationDegrees);
static void timerOutputSetup(void);
static void timer4Setup(void);
static void drive(float power);

void TIM3_C1_Init(void);
void TIM2_C3_Init(void);
void TIM3_IRQHandler(void);

enum carStates
{
	STOPPED,
	DRIVING,
	TURNING,
};

static enum carStates currentState = DRIVING;
static enum carStates prevState = STOPPED;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  pinSetup();
  timer2Setup();
  timer2ChannelSetup();
  timer4Setup();
  TIM3_C1_Init();
  while (1)
  {
	  double distance = timespan / 58;
	  switch(currentState)
	  {
	  case STOPPED:
		  drive(0);
		  break;
	  case DRIVING:
		  if(distance < 10)
		  {
			  currentState = TURNING;
		  } else if(prevState != currentState)
		  {
			  drive(0.4);
			  prevState = currentState;
		  }
		  break;
	  case TURNING:
		  if(prevState != currentState)
		  {
			  drive(0);
			  prevState = currentState;
		  }
		  if(distance > 10)
		  {
			  currentState = DRIVING;
		  } else
		  {
			  rotateVehicle((HAL_GetTick() % 180) - 90);
			  HAL_Delay(500);
		  }
		  break;
	  default:
		  break;
	  }
  }
}

static void drive(float power)
{
	setDutyCycleChannel1(-power);
	setDutyCycleChannel2(power);
}

static void pinSetup(void)
{
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1;
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5_Msk | GPIO_AFRL_AFRL1_Msk | GPIO_AFRL_AFRL2_Msk);
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL5_Pos) | (1 << GPIO_AFRL_AFRL1_Pos) | (1 << GPIO_AFRL_AFRL2_Pos);

}

static void timer2Setup(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 72 - 1;
	TIM2->ARR = ARR_VALUE - 1;
	TIM2->CNT = 0;
}

static void timer4Setup(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->PSC = 72 - 1;
	TIM4->ARR = 50 - 1;
	TIM4->CNT = 0;

	TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk) ;
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	TIM4->CCR1 |= 0;
	TIM4->CCER |= TIM_CCER_CC1E_Msk;
	TIM4->CCER &= ~TIM_CCER_CC1P_Msk;

	GPIOB->MODER |= GPIO_MODER_MODER6_1;
	GPIOB->MODER &= ~(GPIO_MODER_MODER6_0);

	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6_Msk);
	GPIOB->AFR[0] |= (0x2 << GPIO_AFRL_AFRL6_Pos);

	TIM4->CCR1 = 10 - 1;

	TIM4->CR1 |= TIM_CR1_CEN;
}

static void setDutyCycleChannel1(float power)
{
	if(power > 1) power = 1;
	else if(power < -1) power = -1;
	TIM2->CCR1 = DEGREES_0 + power * (DEGREES_0 - DEGREES_90);
}

static void setDutyCycleChannel2(float power)
{
	if(power > 1) power = 1;
	else if(power < -1) power = -1;
	TIM2->CCR2 = DEGREES_0 + power * (DEGREES_0 - DEGREES_90);
}

static void timer2ChannelSetup(void)
{
	//Configure channel 1
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk) ;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	TIM2->CCR1 |= 0;
	TIM2->CCER |= TIM_CCER_CC1E_Msk;
	TIM2->CCER &= ~TIM_CCER_CC1P_Msk;

	//Configure channel 2
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk | TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk) ;
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIM2->CCR2 |= 0;
	TIM2->CCER |= TIM_CCER_CC2E_Msk;
	TIM2->CCER &= ~TIM_CCER_CC2P_Msk;

	//Configure channel 3
	TIM2->CCMR2 &= ~(TIM_CCMR2_CC3S_Msk | TIM_CCMR2_OC3M_Msk | TIM_CCMR2_OC3PE_Msk);
	TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
	TIM2->CCR3 |= 0;
	TIM2->CCER |= TIM_CCER_CC3E_Msk;
	TIM2->CCER &= ~TIM_CCER_CC3P_Msk;

	TIM2->CR1 |= TIM_CR1_CEN;
}

static void rotateVehicle(int rotationDegrees)
{

	if(rotationDegrees == 0) return;
	int delay;
	float power;
	if(rotationDegrees < 0)
	{
		delay = -(20 + 13.67 * rotationDegrees);
		power = -0.3;
	} else
	{
		delay = 20 + 13.67 * rotationDegrees;
		power = 0.3;
	}
	setDutyCycleChannel1(power);
	setDutyCycleChannel2(power);
	HAL_Delay(delay);
	setDutyCycleChannel1(0);
	setDutyCycleChannel2(0);
}

// CONFIGURE TIM4 FOR RECEIVING INPUT SIGNAL
void TIM3_C1_Init(void){
    //Pin setup rewrite
    GPIOB->MODER |= GPIO_MODER_MODER4_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER4_0;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR4_Msk;

    GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL4_Msk;
    GPIOB->AFR[0] |= (0x2 << GPIO_AFRL_AFRL4_Pos);

    //Timer rewrite
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->PSC = 72 - 1;
    TIM3->ARR = 1000 - 1;
    TIM3->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1F;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1PSC;
    TIM3->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->DIER |= TIM_DIER_CC1IE;
    TIM3->DIER |= TIM_DIER_CC1DE;	//DMA request
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 &= ~TIM_CR1_DIR;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void){
	static int lineHigh = 0;
	// Check the update event flag
    if ((TIM3->SR & TIM_SR_UIF) != 0)
    {
        overflow++;                        // if UIF = 1, increment overflow counter
        TIM3->SR &= ~TIM_SR_UIF;           // clear UIF
    }

    // Check capture event flag
    if ((TIM3->SR & TIM_SR_CC1IF) != 0)
    {
        if(lineHigh)
        {
        	timespan = TIM3->SR - newcounter + 1000 * overflow;
        	lineHigh = 0;
        } else
        {
        	newcounter = TIM3->CCR1;
        	overflow = 0;
        	lineHigh = 1;
        }
    }
    EXTI->PR |= EXTI_PR_PIF4;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
