/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
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
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bleDB.h"
#include "DisplayHandler.h"
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* MDTEST */
  uint8_t               Notify_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */

  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[512];
uint8_t NotifyCharData[512];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */
extern char msg[50];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* MDTEST */
static void Custom_Notify_Update_Char(void);
static void Custom_Notify_Send_Notification(void);

/* USER CODE BEGIN PFP */
void my_Task(void)
{
	uint8_t* ucPayload = NULL;
	uint16_t unDataLen = GetBleTxDataBaseMessageLen();
	HAL_NVIC_ClearPendingIRQ(ADC1_IRQn);

	if (0 < unDataLen)
	{
		memset(UpdateCharData, 0X00, sizeof(UpdateCharData));// need to change the size of UpdateCharData//
		memcpy(UpdateCharData, (uint8_t*)PopFromBleTxDataBase(), unDataLen);
		Custom_Notify_Update_Char();
		//Custom_My_char_notify_Update_Char();
		HAL_Delay(50);
		ClearBleTxDataBase();
		BleDataSetRequestStatus(eBleStatusError);
	}
#if 0
	for(p=0;p<48;p++)
	{
	 	 sprintf((uint8_t*)UpdateCharData,"%.4f %d",FINAL_TEMP[p],p);
 	 Custom_My_char_notify_Update_Char();
 	 HAL_Delay(50);
	}
	q++;

	if (eReadingFromFlash != GetReadingState())
	{

		if (0 == ucCount)
		{
			FlashSavePatientResultData(FINAL_TEMP,1);
			ucCount++;
		}
		else if(10 > ucCount)
		{
			FlashSavePatientResultData(FINAL_TEMP,0);
			ucCount++;
		}
		else
		{
			//DAQfinishBuzzerPlay();
			ucCount = 0;
			//HAL_ADC_Stop_IT(&hadc1);
		}
	}
#endif
}
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* MDTEST */
    case CUSTOM_STM_WRITE_WRITE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_WRITE_WRITE_EVT */

      /* USER CODE END CUSTOM_STM_WRITE_WRITE_EVT */
      break;

    case CUSTOM_STM_NOTIFY_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFY_READ_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFY_READ_EVT */
      break;

    case CUSTOM_STM_NOTIFY_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFY_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFY_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_NOTIFY_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFY_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFY_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */

  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* MDTEST */
 void Custom_Notify_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Notify_UC_1*/

  /* USER CODE END Notify_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_NOTIFY, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Notify_UC_Last*/

  /* USER CODE END Notify_UC_Last*/
  return;
}

void Custom_Notify_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Notify_NS_1*/

  /* USER CODE END Notify_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_NOTIFY, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Notify_NS_Last*/

  /* USER CODE END Notify_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
