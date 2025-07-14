/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rx1, rx2;
uint32_t pwm_value = 0;
uint8_t breathing_direction = 1; // 1 = zwiększanie, 0 = zmniejszanie
uint32_t breathing_counter = 0;
uint8_t breathing_mode = 1; // 1 = oddychanie, 0 = stała jasność
uint8_t input_buffer[10];
uint8_t input_index = 0;
uint8_t brightness_percent = 50; // domyślna jasność 50%
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void SetBrightness(uint8_t percent);
void ProcessInput(uint8_t data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &rx1, 1);
  HAL_UART_Receive_IT(&huart2, &rx2, 1);

  // Uruchomienie PWM na kanale 1 (dioda)
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  // Wyślij instrukcje na start
  char* welcome_msg = "Wpisz liczbe 0-100 i nacisnij Enter aby ustawic jasnosc LED\r\nWpisz 'b' aby wlaczyc tryb oddychania\r\n";
  //HAL_UART_Transmit(&huart1, (uint8_t*)welcome_msg, strlen(welcome_msg), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2, (uint8_t*)welcome_msg, strlen(welcome_msg), HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  continue;
    if (breathing_mode)
    {
      // Efekt oddychania diody
      breathing_counter++;
      if (breathing_counter >= 3000) // Opóźnienie dla płynnego efektu
      {
        breathing_counter = 0;

        if (breathing_direction == 1) // Zwiększanie jasności
        {
          pwm_value += 200; // Krok zwiększania
          if (pwm_value >= 50000) // Maksymalna wartość PWM
          {
            pwm_value = 50000;
            breathing_direction = 0; // Zmieniamy kierunek na zmniejszanie
          }
        }
        else // Zmniejszanie jasności
        {
          if (pwm_value >= 200)
          {
            pwm_value -= 200; // Krok zmniejszania
          }
          else
          {
            pwm_value = 0;
            breathing_direction = 1; // Zmieniamy kierunek na zwiększanie
          }
        }

        // Ustawienie nowej wartości PWM
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_value);
      }
    }

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN_Pin_GPIO_Port, LED_GREEN_Pin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_GREEN_Pin_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN_Pin_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void SetBrightness(uint8_t percent)
{
  if (percent > 100) percent = 100;

  // Przelicz procent na wartość PWM (0-50000)
  uint32_t pwm_val = (uint32_t)((percent * 50000) / 100);

  // Ustaw PWM
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_val);

  // Wyślij potwierdzenie
  char response[50];
  sprintf(response, "Jasnosc ustawiona na: %d%%\r\n", percent);
  HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), HAL_MAX_DELAY);
}

void ProcessInput(uint8_t data)
{
  // Echo znaku
  HAL_UART_Transmit(&huart2, &data, 1, HAL_MAX_DELAY);

  if (data == '\r' || data == '\n') // Enter
  {
    if (input_index > 0)
    {
      input_buffer[input_index] = '\0'; // Zakończ string

      // Sprawdź czy to komenda 'b' dla trybu oddychania
      if (input_buffer[0] == 'b' && input_index == 1)
      {
        breathing_mode = 1;
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nTryb oddychania wlaczony\r\n", 28, HAL_MAX_DELAY);
      }
      else
      {
        // Konwertuj na liczbę
        int value = atoi((char*)input_buffer);
        if (value >= 0 && value <= 100)
        {
          breathing_mode = 0; // Wyłącz tryb oddychania
          brightness_percent = value;
          SetBrightness(brightness_percent);
        }
        else
        {
          HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nBledna wartosc! Wpisz liczbe 0-100\r\n", 38, HAL_MAX_DELAY);
        }
      }

      input_index = 0; // Resetuj bufor
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n> ", 4, HAL_MAX_DELAY);
  }
  else if (data == 8 || data == 127) // Backspace
  {
    if (input_index > 0)
    {
      input_index--;
      HAL_UART_Transmit(&huart2, (uint8_t*)" \b", 2, HAL_MAX_DELAY);
    }
  }
  else if (data >= '0' && data <= '9' && input_index < sizeof(input_buffer) - 1)
  {
    input_buffer[input_index++] = data;
  }
  else if (data == 'b' && input_index < sizeof(input_buffer) - 1)
  {
    input_buffer[input_index++] = data;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    ProcessInput(rx2);
    HAL_UART_Receive_IT(&huart1, &rx2, 1);
  }
  else if (huart->Instance == USART2)
  {
    // Zachowaj oryginalną funkcjonalność dla USART2
    uint8_t tx;
    if (rx1 >= '0' && rx1 <= '9')
    {
      tx = rx1;
      if (tx > '9') tx = '0';
    }
    else
    {
      tx = '!';
    }

    HAL_UART_Transmit(&huart1, &tx, 1, HAL_MAX_DELAY);
    HAL_UART_Receive_IT(&huart2, &rx1, 1);
  }
}
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
