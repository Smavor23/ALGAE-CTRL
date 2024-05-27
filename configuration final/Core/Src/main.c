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
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "LoRa.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define MAX_DATA_SIZE 128
#define MAX_PRINT_SIZE 50
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
LoRa myLoRa;
uint8_t read_data[40];
char LoRa_Data[40];
uint8_t LoRa_stat;
char buffer[200];
uint32_t RxData = 0;
float DataArray[50]; // Tableau pour stocker les données séparées
int size = sizeof(DataArray) / sizeof(DataArray[0]); // Taille du tableau

float Temperature = 1;													/////
float Conductivity = 1;													/////
float Salinity = 1;														/////
float TDS = 1;															/////
float PH = 1;

float Previous_Temperature;
float Previous_Conductivity;
float Previous_Salinity;
float Previous_TDS;
float Previous_PH;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
	memset(LoRa_Data, 0, sizeof(LoRa_Data)); // Initialise tous les éléments avec des zéros
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  myLoRa = newLoRa();
  LoRa new_LoRa;
  myLoRa.CS_port         = GPIOB;
  myLoRa.CS_pin          = GPIO_PIN_6;
  myLoRa.hSPIx           = &hspi1;

  if(LoRa_init(&myLoRa)==LORA_OK){
	  LoRa_stat = 1;
  }else {
      LoRa_stat = 0; // Échec de l'initialisation
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
 	  int bytes_received = LoRa_receive(&myLoRa, read_data, sizeof(read_data));
 	  if (LoRa_stat == 1) {
 	              receiveAndProcessLoRaFrame(bytes_received, read_data, &RxData, DataArray, size);
 	              Previous_Temperature = DataArray[0] / 100;
 	              Previous_Conductivity = DataArray[1];
 	              Previous_Salinity = DataArray[2];
 	              Previous_TDS = DataArray[3];
 	              Previous_PH = DataArray[4]/100;
          } else {
	              HAL_UART_Transmit(&huart2, (uint8_t *)"LoRa n'est pas initialisé\n", strlen("LoRa n'est pas initialisé\n"), HAL_MAX_DELAY);
          }

       HAL_UART_Transmit(&huart2, (uint8_t *)"______________________ Les valeurs stocké ________________________\n", strlen("______________________ Les valeurs stocké ________________________\n"), HAL_MAX_DELAY);
       snprintf(buffer, sizeof(buffer), "\nTemperature : %.2f °C\nConductivity : %.2f us/cm\nSalinity: %.2f mg/L\nTDS: %.2f mg/L\nPH: %.2f ph \r\n", Previous_Temperature, Previous_Conductivity, Previous_Salinity, Previous_TDS, Previous_PH);
       HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
       HAL_UART_Transmit(&huart2, (uint8_t *)"______________________________________________\n", strlen("______________________________________________\n"), HAL_MAX_DELAY);

 	 lireTensionBatterie();
 	 lireTensionStable();

 	  HAL_Delay(1000);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
