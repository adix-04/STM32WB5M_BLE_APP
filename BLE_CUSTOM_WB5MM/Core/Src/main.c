/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "ipcc.h"
#include "rf.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DisplayHandler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
_sDeviceInfoData sDeviceInfo					= {0};
static _eOperationMode eOpMode					= eUnknownMode;
static _eBleConnectionStatus eBleCurrentStatus	= eBleStatusUnknown;
static uint8_t ucReadingTrack 					= 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static void UpdateConnectionStatusOnDisplay();

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float FINAL_TEMP[48];
char msg[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint8_t handle_req = 0;


void handle_flash_write()
{
	handle_req = 1;
}
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
  /* Config code for STM32_WPAN (HSE Tuning must be done before system clock configuration) */
  MX_APPE_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* IPCC initialisation */
  MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM16_Init();
  MX_RF_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init code for STM32_WPAN */
  MX_APPE_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  FlashReadDeviceId(&sDeviceInfo);

  C_METwelcomeLogo();
  DeviceInfoLogo();
  SetDisplayStatus(eDisplayNotConnected);
  RestoreConfigDataFromFlash();

  DisplayDAQ_Id();

  while (1)
  {
//	  if (eOpMode == eCOnfigMode)
//	  	{
//	  		ConfigurationModeHandler();
//	  	}
//	  		/* Here the live data reading can be tracked and controlled */
//	  	else if ( eReadingStatusReadingInProgrs == GetReadingStatus() && (ucReadingIterator))
//	  	{
//	  		ProcessLiveReading();
//	  	}
//	  		/* Flash data reading */
//	  	else if ( eReadingFromFlash == GetReadingState())
//	  	{
//	  		ProcessReadingFromFlash();
//	  		HAL_Delay(250);
//	  	}
    UpdateConnectionStatusOnDisplay();
	BlePacketHandler();
    /* USER CODE END WHILE */
    MX_APPE_Process();

    /* USER CODE BEGIN 3 */


    //HAL_Delay(1);
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMHIGH);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS|RCC_PERIPHCLK_RFWAKEUP;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_HSE_DIV1024;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/* USER CODE BEGIN 4 */
_eBleConnectionStatus GetBleConnectionStatus()
{
	return eBleCurrentStatus;
}

static void UpdateConnectionStatusOnDisplay()
{
	static _eBleConnectionStatus ePrvStatus = eBleStatusUnknown;

	if (eOpMode == eCOnfigMode)
	{
		return;
	}

	if ((eBleStatusUnknown == GetBleConnectionStatus()) ||
									(ePrvStatus == GetBleConnectionStatus()))
	{
		return;
	}

	ePrvStatus = GetBleConnectionStatus();

	switch(ePrvStatus)
	{
		case eBleStatusDisConnected:
			UpdateDisconnectedMsg();
			break;

		case eBleStatusConnected:
			UpadteConnectedStatus();
			break;

		case eBleStatusUnknown:
		default:
			break;
	}

}
bool GetReadingIterationStatus()
{
	bool blBretVal = false;

	if ((0 < ucReadingTrack) && (MAX_READING_ITERATION > ucReadingTrack))
	{
		blBretVal = true;
	}

	return blBretVal;
}
void GetDeviceInfo(_sDeviceInfoData* sDeviceInfoData )
{
	memcpy(sDeviceInfoData, &sDeviceInfo, sizeof(_sDeviceInfoData));
	//sDeviceInfoData = &sDeviceInfo;
}
void SetBleConnectionStatus(_eBleConnectionStatus eBleStatus)
{
	eBleCurrentStatus = eBleStatus;
}

/*-----------------------------------------------------------------------------
//Purpose   : Set the error status of the device
//Inputs    : None
//Outputs   : None
//Return    : eErrorStatus - status
//Notes     : None
-----------------------------------------------------------------------------*/
static void ProcessBattery(void)
{
	static uint32_t ulTimeNext 	= 0;
	uint32_t ulBatUpdate		= 0;

	if (eReadingStatusReadingInProgrs == GetReadingStatus())
	{
		return;
	}

	if (ulTimeNext < HAL_GetTick())
	{
		ulTimeNext 	= HAL_GetTick() + BATTERY_UPDATE_PERIOD;
		UpdateBatteryVoltageOnDisplay();
	}

	if (10 > GetBatteryPercentage())
	{
		SetDaqErrorStatus(eErrorBatLOw);
	}
	else
	{
		ClearDaqErrorStatus(eErrorBatLOw);
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
#ifdef USE_FULL_ASSERT
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
