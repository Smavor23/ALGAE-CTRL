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
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "LoRa.h"
#include <math.h>
#include <stdio.h>
#include "sim.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define MAX_DATA_SIZE 128
#define MAX_PRINT_SIZE 50
#define DURATION_MS (10 * 60 * 1000)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
LoRa myLoRa;
uint8_t read_data[40];
char LoRa_Data[40];
char LoRa_Status [30];
uint8_t LoRa_stat;
char buffer[200];
uint32_t RxData = 0;
float DataArray[60]; // Tableau pour stocker les données séparées
int size = sizeof(DataArray) / sizeof(DataArray[0]); // Taille du tableau

float Temperature = 1;													/////
float Conductivity = 1;													/////
float Salinity = 1;														/////
float TDS = 1;															/////
float PH = 1;
float Chlorophyll = 1;

float Previous_Temperature = 1;
float Previous_Conductivity = 1;
float Previous_Salinity = 1;
float Previous_TDS = 1;
float Previous_PH = 1;
float Previous_Chlorophyll = 1;



//************************* SIM *******************************
//char url[] = "http://demo.thingsboard.io/api/v1/9a4gm89e9r7kb5kll4ms/telemetry";

char ATcommand[200];
int TensionBatt;
int TensionStable;
char data_json[250];
int rssi;
//double longitude = -7.14234967;
//double latitude = 33.60038667;
//char nmea[] = "+CGPSINFO:3113.343286,N,12121.234064,E,250311,072809.3,44.1,0.0,0";
float latitudeDecimal;
float longitudeDecimal;
char buffer_gps[100];

//uint8_t buffer_sim;
char data_gps[60];

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
  debugPrintln("------------------------------------------------------ SMART ALGAE CTRL ------------------------------------------------------");
  HAL_Delay(30000);
  SIM_INIT();
  //***************************************** SIM ***************************************
  /*sendATCommandAndWaitForResponse_gps("AT+CGPSINFO\r\n", "OK", 3000, buffer_gps);
  HAL_Delay(1000);
  extractData(buffer_gps, data_gps);
  Coordinates coords = parseNMEA(data_gps);
  latitudeDecimal = convertDMSToDecimal(coords.latitude, coords.latDirection);
  longitudeDecimal = convertDMSToDecimal(coords.longitude, coords.lonDirection);
  sendATCommandAndWaitForResponse("AT+CGPS=0\r\n", "OK", 3000);
  HAL_Delay(500);*/
  //_____________________________________________________________________________________
  while (1)
  {
    /* USER CODE END WHILE */
	  uint32_t startTime = HAL_GetTick();
	    while ((HAL_GetTick() - startTime) < DURATION_MS)
	    {
	        // Appel de la fonction pour recevoir des données via LoRa
	    	//HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
	     	  int bytes_received = LoRa_receive(&myLoRa, read_data, sizeof(read_data));
	     	  if (LoRa_stat == 1) {
	     	              receiveAndProcessLoRaFrame(bytes_received, read_data, DataArray, size);
	     	              Previous_Temperature = DataArray[0] / 100;
	     	              Previous_Conductivity = DataArray[1];
	     	              Previous_Salinity = DataArray[2];
	     	              Previous_TDS = DataArray[3];
	     	              Previous_PH = DataArray[4]/100;
	     	              Previous_Chlorophyll = DataArray[5];
	     	              strcpy(LoRa_Status, "LoRa is Ok");
	              } else {
	    	              HAL_UART_Transmit(&huart2, (uint8_t *)"LoRa n'est pas initialisé\n", strlen("LoRa n'est pas initialisé\n"), HAL_MAX_DELAY);
	    	              strcpy(LoRa_Status, "LoRa is not Ok");
	              }

	           HAL_UART_Transmit(&huart2, (uint8_t *)"\n______________________ Les valeurs stocké ________________________\n", strlen("______________________ Les valeurs stocké ________________________\n"), HAL_MAX_DELAY);
	           snprintf(buffer, sizeof(buffer), "\nTemperature : %.2f °C\nConductivity : %.2f us/cm\nSalinity: %.2f mg/L\nTDS: %.2f mg/L\nPH: %.2f ph \r\nChlorophyll: %.2f ug/L \r\n", Previous_Temperature, Previous_Conductivity, Previous_Salinity, Previous_TDS, Previous_PH, Previous_Chlorophyll);
	           HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
	           HAL_UART_Transmit(&huart2, (uint8_t *)"______________________________________________\n", strlen("______________________________________________\n"), HAL_MAX_DELAY);

	     	  TensionBatt = lireTensionBatterie();
	     	  TensionStable = lireTensionStable();

	     	  HAL_Delay(1000);
	    }


 	  //******************************************************************* SIM *******************************************************
 	  rssi = sendATCommandAndWaitForResponse_signalquality("AT+CSQ\r\n", "OK", 3000);
 	  snprintf(data_json, sizeof(data_json), "{\"temperature\":%.2f, \"Conductivity\":%.2f, \"Salinity\":%.2f, \"TDS\":%.2f, \"PH\":%.2f, \"TensionBatt\":%d, \"TensionStable\":%d, \"rssi\":%d, \"LoRa_Status\":\"%s\", \"Chlorophyll\":%.2f}", Previous_Temperature, Previous_Conductivity, Previous_Salinity, Previous_TDS, Previous_PH, TensionBatt, TensionStable, rssi, LoRa_Status, Previous_Chlorophyll);
 	  HTTPConnect(data_json);

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
