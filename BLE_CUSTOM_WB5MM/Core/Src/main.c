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
#include "FlashMemory.h"
//#include "usart.h"
#include "xmodem.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t ucRxBuffer[12]							= {0};
static _eOperationMode eOpMode					= eUnknownMode;
static _eReadingStatus eDataAqucisionStatus 	= eReadingStatusUnknown;
static bool ucReadingIterator					= false;
static uint8_t ucReadingTrack 					= 0;
static _eBleConnectionStatus eBleCurrentStatus	= eBleStatusUnknown;
_sDeviceInfoData sDeviceInfo					= {0};
bool bPasswordGetter							= false;
bool bOptionGetter								= false;
bool bDeviceIdGetter							= false;
uint8_t ucpDevId[LENGTH_OF_DEV_ID]				= {'\0'};
bool bBackOption								= false;
_ConfigModeStatus eConfigModeCurrentStatus		= eConfigModeUnknown;
uint8_t ucReceivedLen							= 0;
bool bUartReceived								= false;
uint8_t ucEchoBuffer[12]						= {'\0'};

extern char* ucpWearableSize[];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static void ClearReadingFlags();
static void UpdateConnectionStatusOnDisplay();
static void ConfigurationModeHandler();
static void ProcessLiveReading();

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
long double R_TEMP;
long double curve;
long double final_adc_values[48];
long double mode_values[48][60];
long double adc_readings[48][60];
long double value=0;
float FINAL_TEMP[48];
long double e1=0;
int coeffnum;     //s
long double coeff,err_factor;
int R25[48];
long double resistsnce[48];
__IO int ADC_DATA[4];
__IO int i=0;
char msg[100];
__IO int sample=0;
int z=0;
__IO int max[48];
__IO int mode[48][60];
int  count[48];
int probe_no=0;
int j,k,p,x;
int repeat[48][60];
long double final_values[48][60];
int flag=0;
int total;
long double a[48];
long double b[48];
long double c[48];
long double d[48];
float R[48];
long double avg_adc_ref[48];
long double e1;
long double adc_ref[48][60];
long double mode_adc_ref[48][60];
long double final_adc_ref[48][60];
int n=60;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void conv_cmplt_tasks(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (eOpMode != eCOnfigMode)
	{
		if ((NULL != strstr((const char *)ucRxBuffer, (const char *)"CONFIGMODE")) &&
					(eReadingStatusReadingInProgrs != GetReadingStatus()) && (eReadingFromFlash != GetReadingStatus()))
		{
			eOpMode = eCOnfigMode;
			memset(ucRxBuffer, 0x00, 10);
		}
		else
		{
			HAL_UART_Abort_IT(&huart1);

			if(HAL_OK != (HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, sizeof("configmode"))))
			{
				while(1);
			}
		}
	}
	else
	{
		bUartReceived = true;
		ucReceivedLen = huart->RxXferSize;
	}
}

static uint8_t handle_req = 0;

void handle_flash_write()
{
	handle_req = 1;
}
void handle_error_case()
{
	static uint32_t next_hal_time = 0;
	static uint8_t track_time = 0;

	if(handle_req)
	{
		if(next_hal_time < HAL_GetTick())
		{
			next_hal_time = HAL_GetTick() + 1000;
			track_time++;

			if(5 == track_time)
			{
				FlashErasePatientResultData();
				handle_req = 0;
				next_hal_time = 0;
				track_time = 0;
			}
		}

	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	R[0]  = 7500;
	R[1]  = 7500;
	R[2]  = 7500;
	R[3]  = 7500;
	R[4]  = 7500;
	R[5]  = 7500;
	R[6]  = 7500;
	R[7]  = 7500;
	R[8]  = 7500;
	R[9]  = 7500;
	R[10] = 7500;
	R[11] = 7500;
	R[12] = 7500;
	R[13] = 7500;
	R[14] = 7500;
	R[15] = 7500;
	R[16] = 7500;
	R[17] = 7500;
	R[18] = 7500;
	R[19] = 7500;
	R[20] = 7500;
	R[21] = 7500;
	R[22] = 7500;
	R[23] = 7500;
	R[24] = 7500;
	R[25] = 7500;
	R[26] = 7500;
	R[27] = 7500;
	R[28] = 7500;
	R[29] = 7500;
	R[30] = 7500;
	R[31] = 7500;
	R[32] = 7500;
	R[33] = 7500;
	R[34] = 7500;
	R[35] = 7500;
	R[36] = 7500;
	R[37] = 7500;
	R[38] = 7500;
	R[39] = 7500;
	R[40] = 7500;
	R[41] = 7500;
	R[42] = 7500;
	R[43] = 7500;
	R[44] = 7500;
	R[45] = 7500;
	R[46] = 7500;
	R[47] = 7500;
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
	  if (eOpMode == eCOnfigMode)
	  	{
	  		ConfigurationModeHandler();
	  	}
	  		/* Here the live data reading can be tracked and controlled */
	  	else if ( eReadingStatusReadingInProgrs == GetReadingStatus() && (ucReadingIterator))
	  	{
	  		ProcessLiveReading();
	  	}
	  		/* Flash data reading */
	  	else if ( eReadingFromFlash == GetReadingState())
	  	{
	  		ProcessReadingFromFlash();
	  		HAL_Delay(250);
	  	}
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(sample==-1)
	{
		j++;
		HAL_Delay(100);
		HAL_ADC_Start_IT(&hadc1);

		if (j==16)
		{
			sample++;
			sprintf(msg,"\n\rIteration No %d",sample);
			HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
			j = 0;
		}
	}
	else
	{
		HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_RESET);
				adc_readings[j][sample]=(long double)ADC_DATA[2];	//readings from 0 to 15th probes(mux1)
				adc_readings[j+16][sample]=(long double)ADC_DATA[0];//readings from 16 to 32th probes(mux2)
				adc_readings[j+32][sample]=(long double)ADC_DATA[1];//readings from 16 to 32th probes(mux2)
				adc_ref[j][sample]=(long double)(ADC_DATA[3]);
				adc_ref[j+16][sample]=(long double)(ADC_DATA[3]);
				adc_ref[j+32][sample]=(long double)(ADC_DATA[3]);
	//	if (j==0)
	//	{
		sprintf(msg,"Readings from %dth (mux1)=%d \n",j,ADC_DATA[2]);
		HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
		sprintf(msg,"Readings from %dth (mux2)=%d \n",j+16,ADC_DATA[0]);//readings from 17th (mux2)
		HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
		sprintf(msg,"Readings from %dth (mux3)=%d \n",j+32,ADC_DATA[1]);
		HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
	//}


	/*	if (j==1)
				{
				sprintf(msg,"Readings from %dth (mux1)=%d \n",j,ADC_DATA[1]);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				sprintf(msg,"Readings from %dth (mux2)=%d \n",j+16,ADC_DATA[0]);//readings from 17th (mux2)
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				sprintf(msg,"Readings from %dth (mux1)=%d \n",j+32,ADC_DATA[2]);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
			}  */
	/* ___________________________________________________________________________________________________ */

		j++;

		if (j == 16)
		{
			sample++;
			sprintf(msg,"\r\nIteration No. %d ",sample);
			HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
			j=0;
		}
		/* When 60 samples of each probes are taken */
		if(sample == n)
		{
			sample =-1;
			for(probe_no = 0;probe_no < 48;probe_no++)
			{
				/* Finding the maximum no of occurrence of each probe's samples */
				k = 0;
				max[probe_no]=0;
				for (i = 0; i < (n-1); i++)
				{
					mode[probe_no][i] = 1;

					for (z= (i + 1); z < n; z++)
					{
						if (adc_readings[probe_no][i] == adc_readings[probe_no][z])
						{
							mode[probe_no][i]++;
						}
					}
					if ((mode[probe_no][i]) > max[probe_no])
					{
						max[probe_no]= mode[probe_no][i];
					}
				}
			}
			for(probe_no = 0;probe_no < 48;probe_no++)
			{
				count[probe_no] = 0;
				k = 0;
				x = 0;
				for(i = 0;i < n; i++)
				{
					/* choosing the max occurred adc value as well as its nearby values */
					if(((mode[probe_no][i]) + 4) > (max[probe_no]))
					{
						mode_values[probe_no][k] = adc_readings[probe_no][i];
						mode_adc_ref[probe_no][k] = adc_ref[probe_no][i];

						if(k == 0)
						{
							final_values[probe_no][x] = mode_values[probe_no][k];
							final_adc_ref[probe_no][x] = mode_adc_ref[probe_no][k];
							repeat[probe_no][x] = mode[probe_no][i];
							x++;
							count[probe_no]++;

						}
						else
						{
							for ( z = 0;z < k; z++)
							{
								if (mode_values[probe_no][k] == mode_values[probe_no][z])
								{
									flag = 1;
								}
							}

							if (flag == 0)
							{
								final_values[probe_no][x] = mode_values[probe_no][k];
								final_adc_ref[probe_no][x] = mode_adc_ref[probe_no][k];
								repeat[probe_no][x] = mode[probe_no][i];
								x++;
								count[probe_no]++;
							}
						}

						flag = 0;
						k++;
					}
				}
			}
			for(probe_no = 0; probe_no < 48; probe_no++)
			{
				total = 0;
				avg_adc_ref[probe_no] = 0;
				final_adc_values[probe_no] = 0;

				for(p = 0;p < count[probe_no]; p++)
				{
					final_adc_values[probe_no] =  ((final_values[probe_no][p]) * repeat[probe_no][p]) + final_adc_values[probe_no];
					avg_adc_ref[probe_no]=((final_adc_ref[probe_no][p]) * repeat[probe_no][p]) + avg_adc_ref[probe_no];
					total = total + repeat[probe_no][p];
				}

				value = 0;
				avg_adc_ref[probe_no] = avg_adc_ref[probe_no] / total;
				final_adc_values[probe_no] = final_adc_values[probe_no] / total;
				//finding the curve for error factor
				err_factor = ((.1 * exp(-((final_adc_values[probe_no] - 32768) * (final_adc_values[probe_no] - 32768)) / (7000000))) + (0.00000000000001 * (final_adc_values[probe_no] * final_adc_values[probe_no] * final_adc_values[probe_no]))) / 1.358;

				R_TEMP = ((R[probe_no]) * ((avg_adc_ref[0] / (final_adc_values[probe_no] - (21*err_factor))) - 1));
				//R_TEMP=((R[probe_no])*((avg_adc_ref[0]/(final_adc_values[probe_no]))-1));
				e1 = logl(R_TEMP);
				value = a[probe_no] + (b[probe_no] * e1) + (d[probe_no] * e1 * e1 * e1) + (c[probe_no] * e1 * e1);
				value = 1.00 / value;
				value = (value - 273.15);
				//sprintf(msg,"\n\rtemprature from probe %d = %Lf \n\r",probe_no,value);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				FINAL_TEMP[probe_no] = value;

				//FINAL_TEMP[probe_no] = ucReadingTrack;
				//sprintf(msg,"final_adc=%Lf \n\r",final_adc_values[probe_no]);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				sprintf(msg,"count=%d \n\r",total);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				sprintf(msg,"MAX=%d \n\r",max[probe_no]);
				HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
				count[probe_no] = 0;
			}

			ucReadingIterator = true;
			ucReadingTrack++;

			sprintf(msg,"\n\n\r Data set No %d measurement completed \n",ucReadingTrack);
			HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);

			HAL_GPIO_WritePin(	GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(	GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
			}
			else
			{
				switch (j)
				{
				case 7:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 6:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 5:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 4:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 3:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 2:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 1:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 0:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

					break;
				case 15:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 14:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 13:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 12:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 11:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 10:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 9:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				case 8:
					HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

					break;
				}

			HAL_Delay(100); // need to chamge the HAL delay in ISR
			HAL_GPIO_WritePin(GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
			HAL_ADC_Start_IT(&hadc1);
		}
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Handle live reading process
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void ProcessLiveReading()
{
	if (MAX_READING_ITERATION <= ucReadingTrack)
	{
		FlashSavePatientResultData(FINAL_TEMP,1);
		IncreasePercentage(false);
		BuzzerPlay(3);
		ClearReadingFlags();
	}
	else
	{
		IncreasePercentage(false);
		ucReadingIterator = false;
		/*
		 * live reading each iteration
		 */
		// swap function call order
		StartBuildingScreeningPayload();
		FlashSavePatientResultData(FINAL_TEMP,1);
		//UpdateBatteryVoltageOnDisplay();
	}
}

void ProcessDataAquesitionRequest()
{
	static uint8_t run = 0;/* Backward compactability */

	SetDisplayStatus(eDisplayReadingStarted);

	if(run == 0)
	{
		HAL_GPIO_WritePin(	GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);

		if ( HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DATA, 4) != HAL_OK)
		{
			Error_Handler();
		}

		run=1;
	}
	else
	{
		HAL_GPIO_WritePin(	GATE_GPIO_Port, GATE_Pin, GPIO_PIN_SET);
		HAL_ADC_Start_IT(&hadc1);
		SetReadingStatus(eReadingStatusReadingInProgrs);
		SetRedaingState(eLiveReading);
	}
}

_eBleConnectionStatus GetBleConnectionStatus()
{
	return eBleCurrentStatus;
}
/*-----------------------------------------------------------------------------
//Purpose   : Set reading status
//Inputs    : eRequest - Set reading status
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetReadingStatus(_eReadingStatus eRequest)
{
	eDataAqucisionStatus = eRequest;
}
/*-----------------------------------------------------------------------------
//Purpose   : Set reading status
//Inputs    : None
//Outputs   : None
//Return    : Get reading status
//Notes     : None
-----------------------------------------------------------------------------*/
_eReadingStatus GetReadingStatus()
{
	return eDataAqucisionStatus;
}
/*-----------------------------------------------------------------------------
//Purpose   : Clear Reading flags
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void ClearReadingFlags()
{
	UpadteDaqCompletedStatus();

	ucReadingTrack = 0;
	SetReadingStatus(eReadingStatusUnknown);
	HAL_ADC_Stop_IT(&hadc1);
}

/*-----------------------------------------------------------------------------
//Purpose   : Handle config mode
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void ConfigurationModeHandler()
{
	bool bPrintDone				= false;
	bool bModeSwitch			= false;
	bool bDeviceIdStored		= false;
	bool bWdSizeGet				= false;
	bool bWearbleIdGot			= false;

	SetDisplayStatus(eDisplayConfigMode);

	uart_transmit_str((uint8_t*)"\r\n\n   Please enter the authentication key  :- \r\n");
	HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, sizeof("12345")-1);
	eConfigModeCurrentStatus = eConfigModePasswordConfirm;

	/*
	 * ulTimeOutFinder = GetHal
	 */

	while(1)
	{
		/*
			ulTimeOutFinder = 8000+ HAL_GetTick();
		*/
		while(!bUartReceived)
		{
			if (bModeSwitch)
			{
				bModeSwitch = false;
				break;
			}

			/* This portion will be used for implimenting time out */
			/*
			else if (ulTimeOutFinder < HAL_GetTick())
			{
				break;
			}
			Need proper implimentation
			*/
		}

		switch (eConfigModeCurrentStatus)
		{
			case eConfigModePasswordConfirm:
				if (NULL != strstr((const char *)ucRxBuffer, (const char *)"12345"))
				{
					eConfigModeCurrentStatus = eConfigModeMainMenu;
					bModeSwitch = true;
				}
				else
				{
					uart_transmit_str((uint8_t*)"\r\n\n   Wrong Password, Please enter the correct password:-\r\n");
					HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, sizeof("12345"));
				}
			break;
			case eConfigModeMainMenu:
				if (!bPrintDone)
				{
					uart_transmit_str((uint8_t*)"\r\n\n   Please select by pressing the corresponding option number:- \r\n");
					uart_transmit_str((uint8_t*)"     1. Enter Device Settings\r\n");
					uart_transmit_str((uint8_t*)"     2. Enter Wearable Settings\r\n");
					uart_transmit_str((uint8_t*)"     3. Get Device Info\r\n");
					uart_transmit_str((uint8_t*)"     4. Get Wearable Info\r\n");
					HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					bPrintDone = true;
				}
				else
				{
					uint8_t ucUserOption = ucRxBuffer[0] - '0';

					if ((ucUserOption > 0) && (ucUserOption < 5))
					{
						eConfigModeCurrentStatus = ucUserOption + 2;
						bPrintDone = false;
						bModeSwitch = true;
					}
					else
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Wrong option,Please retry\r\n");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					}
				}
			break;
			case eConfigModeDeviceIdSet:
				if (!bPrintDone)
				{
					uart_transmit_str((uint8_t*)"\r\n\n   Enter a valid Device ID:-\n\r");
					HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, LENGTH_OF_DEV_ID);
					bPrintDone = true;
				}
				else
				{
					if ((LENGTH_OF_DEV_ID == ucReceivedLen) && (ucRxBuffer[0] == 'M') && (ucRxBuffer[1] == 'D'))
					{
						uart_transmit_str((uint8_t*)"\r\n\n   The Device ID is set as ");
						uart_transmit_str((uint8_t*)ucRxBuffer);
						memcpy(sDeviceInfo.ucDeviceId, ucRxBuffer, LENGTH_OF_DEV_ID);
						FlashSaveDeviceInfo((uint8_t*)&sDeviceInfo);
						DisplayDAQ_Id();
						bDeviceIdStored = true;
						uart_transmit_str((uint8_t*)"\r\n\n   Please restart the device ! \r\n");
						UpdateRestartDevice();

						while(1);
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					}
					else if (true == bDeviceIdStored)
					{
						uint8_t ucUserOption = ucRxBuffer[0] - '0';

						if (1 == ucUserOption)
						{
							eConfigModeCurrentStatus = eConfigModeMainMenu;
							bPrintDone 		= false;
							bModeSwitch 	= true;
							bDeviceIdStored = false;
						}
					}
					else
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Entered Device ID is invalid \r\n   (Start with 'MD' and should exactly contain 10 characters)");
						bModeSwitch = true;
						bPrintDone = false;
					}
				}
			break;
			case eConfigModeWearableIdSet:
				if (!bPrintDone)
				{
					if(!bWearbleIdGot)
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Enter a valid Wearable ID :- \r\n");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, sizeof(sDeviceInfo.ucDeviceId));
						bPrintDone = true;
					}
					else
					{
						uart_transmit_str((uint8_t*)"\r\n   Please select from the options given \r\n");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
						bPrintDone = true;
					}
				}
				else
				{
					if ((ucRxBuffer[0] == 'W') && (ucRxBuffer[1] == 'D') && (LENGTH_OF_DEV_ID == ucReceivedLen))
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Entered Wearable ID is ");
						uart_transmit_str((uint8_t*)ucRxBuffer);
						memcpy(sDeviceInfo.ucWearableId, ucRxBuffer, LENGTH_OF_DEV_ID);
						bWearbleIdGot = true;
						uart_transmit_str((uint8_t*)"\r\n\n   Please press the option number to select the WD size");
						uart_transmit_str((uint8_t*)"\r\n     1. Small\n\r     2. Medium\n\r     3. Large\n\r     4. XLarge\n\r     5. XXLarge\n\r     6. XXXLarge\n\r     7. FULL\n\r");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					}
					else if (!bWearbleIdGot)
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Entered Wearable ID is invalid \r\n   (Start with 'WD' and should exactly contain 10 characters)");
						bPrintDone  = false;
						bModeSwitch = true;
					}
					else
					{
						uint8_t ucWdSize = ucRxBuffer[0] - '0';

						if((ucWdSize > 0) && (ucWdSize < 8))
						{
							sDeviceInfo.eWearableSize = ucWdSize;
							bWdSizeGet = true;
							uart_transmit_str((uint8_t*)"\r\n\n   Please upload the text file through XMODEM");
							uart_transmit_str((uint8_t*)"\r\n   File -> Transfer -> XMODEM -> Send");
							uart_transmit_str((uint8_t*)"\r\n   Attention: Make sure you are using the standard template \r\n\n");
							xmodem_receive();
						}
						else
						{
							uart_transmit_str((uint8_t*)"\r\n\n   Wrong option,Please retry\r\n");
							bPrintDone  = false;
							bModeSwitch = true;
						}
					}
				}
			break;
			case eConfigModeGetDeviceId:
				if (!bPrintDone)
				{
					uint8_t ucDeviceId[DEVICEID_LEN] = {0};
					memcpy(ucDeviceId, sDeviceInfo.ucDeviceId, DEVICEID_LEN);
					uart_transmit_str((uint8_t*)"\r\n\n   Device ID : ");
					uart_transmit_str((uint8_t*)ucDeviceId);
					uart_transmit_str((uint8_t*)"\r\n   HW Ver    : ");
					uart_transmit_str((uint8_t*)HW_VERSION);
					uart_transmit_str((uint8_t*)"\r\n   SW Ver    : ");
					uart_transmit_str((uint8_t*)SW_VERSION);
					uart_transmit_str((uint8_t*)"\r\n\n   Press '0' to go back \r\n");
					bPrintDone = true;
					HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
				}
				else
				{
					uint8_t ucUserOption = ucRxBuffer[0] - '0';

					if (0 == ucUserOption)
					{
						eConfigModeCurrentStatus = eConfigModeMainMenu;
						bPrintDone = false;
						bModeSwitch = true;
					}
					else
					{
						uart_transmit_str((uint8_t*)"\r\n\n   Press '0' to go back\r\n");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					}
				}
			break;
			case eConfigModeGetWearableId:
				if (!bPrintDone)
				{
					uart_transmit_str((uint8_t*)"\r\n\n   Wearable ID : ");
					uart_transmit_str((uint8_t*)sDeviceInfo.ucWearableId);
					uart_transmit_str((uint8_t*)"\r\n\n   WD ID Size  : ");
					uart_transmit_str((uint8_t*)ucpWearableSize[sDeviceInfo.eWearableSize-1]);

					for (coeffnum = 0; coeffnum < 48; coeffnum++)  //s
					{
						//sprintf(msg,"\n\n \r   a[%d]=%.18Lf",coeffnum,a[coeffnum]);
						HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
						//sprintf(msg,"\n\r   b[%d]=%.18Lf",coeffnum,b[coeffnum]);
						HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
						//sprintf(msg,"\n\r   c[%d]=%.18Lf",coeffnum,c[coeffnum]);
						HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
						//sprintf(msg,"\n\r   d[%d]=%.18Lf",coeffnum,d[coeffnum]);
						HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),100);
					}

					uart_transmit_str((uint8_t*)"\r\n\n   Press '0' to go back\r\n");
					bPrintDone = true;
					HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
				}
				else
				{
					uint8_t ucUserOption = ucRxBuffer[0] - '0';

					if (0 == ucUserOption)
					{
						eConfigModeCurrentStatus = eConfigModeMainMenu;
						bPrintDone = false;
						bModeSwitch = true;
					}
					else
					{
						uart_transmit_str((uint8_t*)"\r\n\   Press '0' to go back\r\n");
						HAL_UART_Receive_IT(&huart1, (uint8_t *)ucRxBuffer, 1);
					}
				}
			break;
		}

		bUartReceived = false;
	}
}
/*-----------------------------------------------------------------------------
//Purpose   : Buzzer finished indication
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BuzzerPlay(int num_times)
{
    for (int i = 0; i < num_times; i++)
    {
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    }
}

void BuzzerPlayPiezo(void)
{
    for (int i = 0; i < 1000; i++)
    {
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);
    }
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
//Outputs   :kklx None
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
void conv_cmplt_tasks(void)
{

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
