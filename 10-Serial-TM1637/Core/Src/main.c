/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "uart.h"
#include "my_dma.h"
#include "oled.h"
#include "tm1637.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// R: PB5, G: PB0, B: PB1
// #define LED_G 0b111111111111111111111111 11111110
// #define LED_B 0b111111111111111111111111 11111101
// #define LED_R 0b111111111111111111111111 11011111

#define LED_R 0xFFFFFFDF
#define LED_G 0xFFFFFFFE
#define LED_B 0xFFFFFFFD

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t ledColorArray[] = {LED_R, LED_G, LED_B};
// static uint8_t ledColorIndex = 0;

extern UART_HandleTypeDef uart1Handle;
extern DMA_HandleTypeDef uart1RxDMAHandle;
uint8_t uartRecvBuf[256];
uint32_t count = 0;

extern TIM_HandleTypeDef htim6;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  
  // UART1 and DMA
  HAL_StatusTypeDef dmaInitStat = InitUART1DMA();
  InitBoardUART();

  // OLED
  // 0x78: OLED i2c address
  OLED_ConfigDisplay(&hi2c1, 0x78);

  HAL_TIM_Base_Start(&htim6);

  if (OLED_InitDisplay() < 0) {
    printf("init OLED screen failed!\r\n");
  } else {
    puts("OLED ok!");
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  puts("hello UARTx!");
  if (dmaInitStat == HAL_OK) {
    printf("DMA init success!\r\n");
  } else {
    printf("DMA init failed!\r\n");
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();

  // recv data from UART in interrupt mode
  // HAL_UART_Receive_IT(&uart1Handle, uartRecvBuf, 16);

  // enable NVIC interrupt handle for USART1
  HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  // enable NVIC interrupt handle for DMA1 rx
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  // enable NVIC interrupt handle for DMA1 tx
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

  // dma tx test
#if 0
  char buf[] = "hello world DMA\r\n";
  HAL_UART_Transmit_DMA(&uart1Handle, (uint8_t *)buf, sizeof(buf));

  HAL_Delay(10);
  // dma rx
  // HAL_UART_Receive_DMA(&uart1Handle, (uint8_t*)uartRecvBuf, sizeof(uartRecvBuf));
  // start rx DMA
  HAL_UARTEx_ReceiveToIdle_DMA(&uart1Handle, (uint8_t*)uartRecvBuf, sizeof(uartRecvBuf));
  // disable DMA half-complete interrupt
  __HAL_DMA_DISABLE_IT(&uart1RxDMAHandle, DMA_IT_HT);
#endif

  tm1637Init();
  tm1637Display("1234", 4);

  

  // OLED_Test();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    GPIOB->ODR = LED_R;
    HAL_Delay(500);
    GPIOB->ODR = LED_G;
    HAL_Delay(500);
    GPIOB->ODR = LED_B;
    HAL_Delay(500);
    count++;
    tm1637DisplayInt(count);

    // sprintf(cbuf, "count: %lu", count);
    // oledShowString(0, 0, cbuf);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&uart1Handle, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return 0;
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