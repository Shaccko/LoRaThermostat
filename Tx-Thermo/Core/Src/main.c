/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*--------------------TRANSMITTER--------------------*/
#include <string.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "LoRa.h"
#include <stdlib.h>
#include <LiquidCrystal.h>
#include "Tx-Commands.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UNAVAILABLE -1
#define FREE 0
#define BUSY 1

#define HEAT 0
#define AC 1
#define OFF -1

#define TEMP_PORT GPIOA

#define MAX_BUTTONS 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
LoRa myLoRa;
uint16_t LoRa_stat = 0;
uint8_t RxBuffer[16];
size_t air_mode = OFF;
uint8_t temperature = 0;
uint8_t temp_setpt = 0;
volatile uint8_t rx_ready = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
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
  MX_SPI1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */



  myLoRa = newLoRa();
  myLoRa.hSPIx = &hspi1;
  myLoRa.CS_port = GPIOB;
  myLoRa.CS_pin = GPIO_PIN_0;
  myLoRa.reset_port = GPIOB;
  myLoRa.reset_pin = GPIO_PIN_1;
  myLoRa.DIO0_port = GPIOB;
  myLoRa.DIO0_pin = GPIO_PIN_10;

  if (LoRa_init(&myLoRa) != LORA_OK) {
	  print("LoRa Failed!");
  }
  LoRa_stat = 1;

  LiquidCrystal(GPIOB, LCD_RS_Pin, 255, LCD_E_Pin,
		  LCD_D7_Pin, LCD_D6_Pin, LCD_D5_Pin, LCD_D4_Pin);
  Buttons button[] = {
		  {FREE, Get_TempUpButtonState, TransmitTEMPUPCommand},
		  {FREE, Get_TempDownButtonState, TransmitTEMPDOWNCommand},
		  {FREE, Get_HeatButtonState, TransmitHEATCommand},
		  {FREE, Get_ACButtonState, TransmitACCommand},
		  {FREE, Get_OffButtonState, TransmitOFFCommand}
  };
  air_mode = OFF;
  char* command;
  LCD_StrInit();
  LoRa_startReceiving(&myLoRa);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  size_t i;
	  if (rx_ready) {
		  command = (char*) RxBuffer;
		  rx_ready = 0;
		  if (atoi(command) != 0) {
			  uint16_t tmp = atoi(command);
			  /* We receive 16 bits, upper 8 bits hold temp, lower 8 bits hold setpoint*/
			  temp_setpt = tmp & 0xFF;
			  temperature = (tmp >> 8) & 0xFF;
			  LCD_UpdateSetpt();
			  LCD_UpdateTemp();
		  }
	  }

	  for (i = 0; i < MAX_BUTTONS; i++) {
		  if (button[i].GetPinState() == FREE && button[i].state == FREE) {
			  button[i].state = BUSY;
			  if (button[i].TransmitMessage()) {
				  /* some success message */
			  }
			  else {
				  /* some failure message */
			  }
		  }

		  if (button[i].GetPinState() == BUSY && button[i].state == BUSY) {
			  button[i].state = FREE;
		  }
		  memset(RxBuffer, 0, sizeof(RxBuffer));
		  HAL_Delay(10); /* Debouncing */

	  }
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

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* SPI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == DIO0_Pin) {
		LoRa_receive(&myLoRa, RxBuffer, 16);
		rx_ready = 1;
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
