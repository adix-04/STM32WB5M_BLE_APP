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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
