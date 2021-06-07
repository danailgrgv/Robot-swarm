#include "main.h"
#include <stdio.h>
#include <string.h>

#define ARR_VALUE 20000
#define DEGREES_90 ARR_VALUE / 20 * 1.28
#define DEGREES_0 ARR_VALUE / 20 * 1.5
#define DEGREES_MINUS_90 ARR_VALUE / 20 * 1.72

volatile int timespan = 0;                              // Total pulse width
volatile int test = 0;
volatile int lastcounter = 0;                           // Timer counter value of the last event

volatile int dutyCycleW1;
volatile int dutyCycleW2;

UART_HandleTypeDef huart2;
SPI_HandleTypeDef hspi2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

static void pinSetup(void);
static void timer2Setup(void);
static void setDutyCycleChannel1(float compareValue);
static void setDutyCycleChannel2(float compareValue);
static void rotateVehicle(int rotationDegrees);
static void timer2ChannelSetup(void);
static void timer4Setup(void);
static void drive(float power);
static void MX_TIM15_Init(void);

static void MX_SPI2_Init(void);

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

  pinSetup();
  timer2Setup();
  timer2ChannelSetup();
  timer4Setup();
  MX_TIM15_Init();
  TIM3_C1_Init();

  //uartSetPins();
  MX_SPI2_Init();



  MX_USART2_UART_Init();
  while (1)
  {
	  int distance = timespan / 58;
	  static int printCounter = 0;
	  if(HAL_GetTick() > 1000 * printCounter)
	  {
		  char buffer[50];
		  sprintf(buffer, "%d\r\n", distance);
		  //HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 100);
		  //sprintf(buffer, "%d\r\n", dutyCycleW1);
		  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 100);
		  printCounter++;
	  }
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
			  int direction = HAL_GetTick() % 2;
			  //rotateVehicle(direction);
			  prevState = currentState;
		  }
		  if(distance > 10)
		  {
			  currentState = DRIVING;
		  } else
		  {

			  rotateVehicle(90);
			  //HAL_Delay(200);
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
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER1_1;
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_0 | GPIO_MODER_MODER1_0);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5_Msk | GPIO_AFRL_AFRL1_Msk);
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL5_Pos) | (1 << GPIO_AFRL_AFRL1_Pos);

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

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{
    //Pin setup rewrite
    GPIOB->MODER |= GPIO_MODER_MODER14_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER14_0;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR14_Msk;

    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH6_Msk;
    GPIOB->AFR[1] |= (0x1 << GPIO_AFRH_AFRH6_Pos);

    GPIOB->MODER |= GPIO_MODER_MODER15_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER15_0;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR15_Msk;

    GPIOB->AFR[1] &= ~GPIO_AFRH_AFRH7_Msk;
    GPIOB->AFR[1] |= (0x1 << GPIO_AFRH_AFRH7_Pos);

    //Timer rewrite
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;

    TIM15->PSC = 72 - 1;
    TIM15->ARR = 1099 - 1;
    TIM15->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM15->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM15->CCMR1 &= ~TIM_CCMR1_IC1F;
    TIM15->CCMR1 &= ~TIM_CCMR1_IC1PSC;
    TIM15->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;
    TIM15->CCER |= TIM_CCER_CC1E;
    TIM15->DIER |= TIM_DIER_CC1IE;
    TIM15->DIER |= TIM_DIER_CC1DE;	//DMA request
    TIM15->DIER |= TIM_DIER_UIE;
    //TIM15->CR1 &= ~TIM_CR1_DIR;

	TIM15->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM15->CCMR1 |= TIM_CCMR1_CC2S_0;
	TIM15->CCMR1 &= ~TIM_CCMR1_IC2F;
	TIM15->CCMR1 &= ~TIM_CCMR1_IC2PSC;
	TIM15->CCER |= TIM_CCER_CC2P | TIM_CCER_CC2NP;
	TIM15->CCER |= TIM_CCER_CC2E;
	TIM15->DIER |= TIM_DIER_CC2IE;
	TIM15->DIER |= TIM_DIER_CC2DE;	//DMA request
	//TIM15->CR2 &= ~TIM_CR2_DIR;
	TIM15->CR1 |= TIM_CR1_CEN;
    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 2);
    NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
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

	TIM2->CR1 |= TIM_CR1_CEN;
}

static void rotateVehicle(int rotationDegrees)
{

	//if(rotationDegrees == 0) return;
	int delay;
	float power;
	if(rotationDegrees == 0)
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
	//setDutyCycleChannel1(0);
	//setDutyCycleChannel2(0);
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
    TIM3->ARR = 65536 - 1;
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
	static int overflow = 0;
	static int newcounter = 0;
	// Check the update event flag
    if ((TIM3->SR & TIM_SR_UIF) != 0)
    {
        overflow++;                        // if UIF = 1, increment overflow counter
        TIM3->SR &= ~TIM_SR_UIF;           // clear UIF
    }

    // Check capture event flag
    else if ((TIM3->SR & TIM_SR_CC1IF) != 0)
    {
        if(lineHigh)
        {
        	timespan = TIM3->CCR1 - newcounter + 65536 * overflow;
        	lineHigh = 0;
        } else
        {
        	overflow = 0;
        	newcounter = TIM3->CCR1;
        	lineHigh = 1;
        }
    }
    EXTI->PR |= EXTI_PR_PIF3;
}



void TIM15_IRQHandler(void){
	static int lineHighW1 = 0;
	static int overflowW1 = 0;
	static int newcounterW1 = 0;
	static int lineHighW2 = 0;
	static int overflowW2 = 0;
	static int newcounterW2 = 0;
	// Check the update event flag
    if ((TIM15->SR & TIM_SR_UIF) != 0)
    {
    	overflowW1++;                        // if UIF = 1, increment overflow counter
    	overflowW2++;
        TIM15->SR &= ~TIM_SR_UIF;           // clear UIF
    }

    // Check capture event flag
    else if ((TIM15->SR & TIM_SR_CC1IF) != 0)
    {
        if(lineHighW1)
        {
        	dutyCycleW1 = ((100 * (TIM15->CCR1 - newcounterW1 + 1099 * overflowW1) / 1099) - 2.9) * 360 /(91.7 - 2.9 + 1);
        	lineHighW1 = 0;
        } else
        {
        	lineHighW1 = 1;
        	newcounterW1 = TIM15->CCR1;
        	overflowW1 = 0;
        }
    }
    else if ((TIM15->SR & TIM_SR_CC2IF) != 0)
    {
        if(lineHighW2)
        {
        	dutyCycleW2 = ((100 * (TIM15->CCR2 - newcounterW2 + 1099 * overflowW2) / 1099) - 2.9) * 360 /(91.7 - 2.9 + 1);
        	lineHighW2 = 0;
        } else
        {
        	lineHighW2 = 1;
        	newcounterW2 = TIM15->CCR2;
        	overflowW2 = 0;
        }
    }
    EXTI->PR |= EXTI_PR_PIF15;
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
  huart2.Init.BaudRate = 9600;
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

static void MX_SPI2_Init(void)
{
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
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

  /*Configure GPIO pins : PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
