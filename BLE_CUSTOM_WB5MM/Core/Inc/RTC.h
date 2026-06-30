//**************************** RTC_HELPER ************************************
// Copyright (c)
// All Rights Reserved
//*****************************************************************************
//
// Summary  : Functions, structures, enums, macros related to RTC Helper
// Note     : None
// :
// :
//
//*****************************************************************************

#ifndef __RTC_H_
#define __RTC_H_

//******************************* Includes ************************************
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "stm32wbxx_hal.h"

//******************************* Macros **************************************

#define TIME_SYNC_RETRY_COUNT        3
#define NTP_SERVER					"time.nist.gov"
#define NTP_SYNC_REPEAT_TIME		 7 // 7 days
#define TIMESTAMP_FORMAT_NTP		"%Y/%m/%d,%H:%M:%S"
#define TIMESTAMP_FORMAT_RTC		"%m-%d-%YT%H:%M:%S:"
#define TIMEDATE_BUFFSIZE       	38
#define CMD_BUFF_SIZE				150
#define DAYS_29						29 // 29 days
#define DAYS_31						31 // 31 days
#define DAYS_28						28 // 28 days
#define DAYS_30						30 // 30 days
#define TIMEOUT_5_SEC				5000
#define RTC_ASYNC_PRE_DIVIDER_VALUE 127
#define RTC_SYNC_PRE_DIVIDER_VALUE  255
#define YEAR_OFFSET                 2000

#define TEST_TIME 					"2023/03/23,17:13:01"


//******************************* Function Declarations ************************

bool GetCurrentTimeInUTC(char* pcTimeStr, uint8_t ucTimebufferLength);
bool SetRtcTime(char *pcTimeStr);
bool GetRtcTime(RTC_TimeTypeDef *psTime);
bool GetRtcDate(RTC_DateTypeDef *psDate);

#endif /* __RTC_H_ */
