#include "uart.h"

UART_HandleTypeDef uart1Handle;

void InitBoardUART(void)
{
    // 1. enable clock
    __HAL_RCC_USART1_CLK_ENABLE();

    // 2. set parameters
    UART_InitTypeDef uart1Init;
    uart1Init.BaudRate = 115200;
    uart1Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1Init.Mode = UART_MODE_TX_RX;
    uart1Init.OverSampling = UART_OVERSAMPLING_16;
    uart1Init.Parity = UART_PARITY_NONE;
    uart1Init.StopBits = UART_STOPBITS_1;
    uart1Init.WordLength = UART_WORDLENGTH_8B;

    
    uart1Handle.Init = uart1Init;
    uart1Handle.Instance = USART1;
    
    HAL_UART_Init(&uart1Handle);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        GPIO_InitTypeDef uartGPIO;
        uartGPIO.Mode = GPIO_MODE_AF_PP;
        uartGPIO.Pin = GPIO_PIN_9;
        uartGPIO.Pull = GPIO_NOPULL;
        uartGPIO.Speed = GPIO_SPEED_HIGH;
        // UART1 TX
        HAL_GPIO_Init(GPIOA, &uartGPIO);

        uartGPIO.Mode = GPIO_MODE_AF_INPUT;
        uartGPIO.Pin = GPIO_PIN_10;
        // UART1 RX
        HAL_GPIO_Init(GPIOA, &uartGPIO);
    }
}