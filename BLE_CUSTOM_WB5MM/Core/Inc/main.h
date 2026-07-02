/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"
#include <stdbool.h>
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define DEVICEID_LEN			10
#define DAQID_LEN				10
#define DAQ_ID					"MD42311000" // It will change
#define WEARABLE_ID				"WD12345678"
#define SW_VERSION				"v5.1"
#define HW_VERSION				"v4.2"
#define WD_ID					"WD00001"
#define WD_SIZE					"XXXL"
#define LENGTH_OF_DEV_ID		10
#define LENGTH_OF_WEARABLE_ID	10
#define MAX_READING_ITERATION	10
#define CONFIG_THROUGH_XMODEM
#define BATTERY_UPDATE_PERIOD		2500

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef enum __eWearableSize
{
	eWearableSizeInvalid 	= 0,
	eWearableSmall			= 1,
	eWearableMedium			= 2,
	eWearableLarge			= 3,
	eWearableXlarge			= 4,
	eWearableXXlarge		= 5,
	eWearableXXXlarge		= 6,
	eWearableFullSize		= 7
}_eWearableSize;

typedef struct __sDeviceInfoData
{
	uint8_t		 	ucDeviceId[DEVICEID_LEN];
	uint8_t 		ucWearableId[DAQID_LEN];
	_eWearableSize	eWearableSize;
} _sDeviceInfoData;

typedef enum __eBleConnectionStatus
{
	eBleStatusUnknown 		= 0,
	eBleStatusDisConnected 	= 1,
	eBleStatusConnected		= 2
}_eBleConnectionStatus;

typedef enum __eOperationMode
{
	eUnknownMode	= 0,
	eCOnfigMode		= 1
}_eOperationMode;

typedef enum __eReadingStatus
{
	eReadingStatusUnknown 			= 0,
	eReadingStatusReqReceived		= 1,
	eReadingStatusReadingInProgrs 	= 2
}_eReadingStatus;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
bool FlashReadDeviceId(_sDeviceInfoData *ucData);
_eBleConnectionStatus GetBleConnectionStatus();
_eReadingStatus GetReadingStatus();
void SetReadingStatus(_eReadingStatus eRequest);
bool GetReadingIterationStatus();
void SetBleConnectionStatus(_eBleConnectionStatus eBleStatus);
bool GetReadingIterationStatus();
void GetDeviceInfo(_sDeviceInfoData* sDeviceInfoData );
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MUX1_ADC_Pin GPIO_PIN_2
#define MUX1_ADC_GPIO_Port GPIOA
#define MUX2_ADC_Pin GPIO_PIN_1
#define MUX2_ADC_GPIO_Port GPIOA
#define GATE_Pin GPIO_PIN_7
#define GATE_GPIO_Port GPIOB
#define LD1_Pin GPIO_PIN_5
#define LD1_GPIO_Port GPIOB
#define A2_Pin GPIO_PIN_12
#define A2_GPIO_Port GPIOA
#define A3_Pin GPIO_PIN_11
#define A3_GPIO_Port GPIOA
#define B2_Pin GPIO_PIN_0
#define B2_GPIO_Port GPIOD
#define B2D1_Pin GPIO_PIN_1
#define B2D1_GPIO_Port GPIOD
#define BUZZER_PIN_Pin GPIO_PIN_6
#define BUZZER_PIN_GPIO_Port GPIOB
#define B1_Pin GPIO_PIN_4
#define B1_GPIO_Port GPIOC
#define MUX3_ADC_Pin GPIO_PIN_8
#define MUX3_ADC_GPIO_Port GPIOA
#define A0_Pin GPIO_PIN_7
#define A0_GPIO_Port GPIOA
#define A1_Pin GPIO_PIN_6
#define A1_GPIO_Port GPIOA
#define VCC_REF_Pin GPIO_PIN_3
#define VCC_REF_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
