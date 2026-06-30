//******************************* GSM_HELPER *********************************
// Copyright (c)
// All Rights Reserved
//*****************************************************************************
//
// File		: RTC.c
// Summary	:
// Note		:
// Author	:
// Date		:
//
//*****************************************************************************

//******************************* Includes ************************************

#include "RTCC.h"
#include "stm32wbxx_hal.h"
#include <stdio.h>
#include <string.h>

#define TIME_STAMP_LEN			12
#define STRING_2_INT(x) 		x-'0'

//******************************* Local Types *********************************
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef gTime;
extern RTC_DateTypeDef gDate;
bool GetRtcTime(RTC_TimeTypeDef *psTime);
bool GetRtcDate(RTC_DateTypeDef *psDate);

//***************************** Private Variables *****************************

//******************************.FUNCTION_HEADER.******************************
//Purpose   : Get RTC time
//Inputs    : None
//Outputs   : psTime - Pointer to store current RTC time
//Return    : bool true / false
//Notes     :
//*****************************************************************************
bool GetRtcTime(RTC_TimeTypeDef *psTime)
{
	bool bRetValue = false;

	if (NULL != psTime)
	{
		if (HAL_OK == HAL_RTC_GetTime(&hrtc, psTime, RTC_FORMAT_BIN))
		{
			bRetValue = true;
		}
	}

	return bRetValue;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose   : Get RTC Date
//Inputs    : None
//Outputs   : psDate - Pointer to store current RTC date.
//Return    : bool true / false
//Notes     :
//*****************************************************************************
bool GetRtcDate(RTC_DateTypeDef *psDate)
{
	bool bRetValue = false;

	if (NULL != psDate)
	{
		if (HAL_OK == HAL_RTC_GetDate(&hrtc, psDate, RTC_FORMAT_BIN))
		{
			bRetValue = true;
		}
	}

	return bRetValue;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose   : Get time in UTC format
//Inputs    : ucTimebufferLength - Length of time buffer
//Outputs   : pcTimeStr - Pointer to store the time
//						 in UTC format (窶弸YYY-MM-DDTHH:MM:SS:MSZ窶�)
//Return    : bool true / false
//Notes     :
//*****************************************************************************
bool GetCurrentTimeInUTC(char* pcTimeStr, uint8_t ucTimebufferLength)
{
	RTC_TimeTypeDef sTime 	= { 0 };
	RTC_DateTypeDef sDate	= { 0 };
	bool bRetValue 			= false;

    do
    {
    	if ((pcTimeStr == NULL) && ucTimebufferLength < TIMEDATE_BUFFSIZE)
    	{
    		break;
    	}

		if (!GetRtcTime(&sTime))
		{
			break;
		}

		if (!GetRtcDate(&sDate))
		{
			break;
		}

		sprintf(pcTimeStr,"%04d-%02d-%02dT%02d:%02d:%02d:%02dZ",
				YEAR_OFFSET + sDate.Year, sDate.Month, sDate.Date,
				sTime.Hours, sTime.Minutes, sTime.Seconds,
				(int)sTime.SubSeconds);

		bRetValue = true;

    } while(0);

    return bRetValue;
}

void ConvertTimeFrmDtring(char* cTime, struct tm *timeStruct)
{
    if (NULL != cTime)
    {
        timeStruct->tm_mday = (STRING_2_INT(cTime[0])) * 10 + STRING_2_INT(cTime[1]);
        timeStruct->tm_mon = (STRING_2_INT(cTime[2])) * 10 + STRING_2_INT(cTime[3]);
        timeStruct->tm_year = (STRING_2_INT(cTime[4]))* 10 + STRING_2_INT(cTime[5]);
        timeStruct->tm_hour = (STRING_2_INT(cTime[6])) * 10 + STRING_2_INT(cTime[7]);
        timeStruct->tm_min = (STRING_2_INT(cTime[8])) * 10 + STRING_2_INT(cTime[9]);
        timeStruct->tm_sec = (STRING_2_INT(cTime[10])) * 10  + STRING_2_INT(cTime[11]);
    }
}

//******************************.FUNCTION_HEADER.******************************
//Purpose   : Set RTC time
//Inputs    : pTimeStr - Pointer to the time string
//				in 窶弸YYY/MM/DD,hh:mm:ssﾂｱzz窶� format
//Outputs   : None
//Return    : bool true / false
//Notes     :
//*****************************************************************************
bool SetRtcTime(char *pcTimeStr)
{
    RTC_TimeTypeDef		sTime   	= { 0 };
    RTC_DateTypeDef		sDate	    = { 0 };
    struct tm			sTimeBuffer	= { 0 };
    bool                bRetVal     = false;
    char cTempBuff[TIME_STAMP_LEN] = {0};

    do
    {
 		if (NULL == pcTimeStr)
    	{
    		break;
    	}

        hrtc.Instance = RTC;
        hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
        hrtc.Init.AsynchPrediv = RTC_ASYNC_PRE_DIVIDER_VALUE;
        hrtc.Init.SynchPrediv = RTC_SYNC_PRE_DIVIDER_VALUE;
        hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
        hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

        if (HAL_RTC_Init(&hrtc) != HAL_OK)
        {
        	Error_Handler();
        }
#if 0
        if ( 0 == strptime(pcTimeStr, "%Y%m%d%H%M%S", &sTimeBuffer))
        {
            break;
        }
#endif
        ConvertTimeFrmDtring(pcTimeStr, &sTimeBuffer);
        memset(&sTime, 0, sizeof(sTime));

        sTime.Hours 	= (uint8_t)sTimeBuffer.tm_hour;
        sTime.Minutes 	= (uint8_t)sTimeBuffer.tm_min;
        sTime.Seconds 	= (uint8_t)sTimeBuffer.tm_sec;
        sTime.TimeFormat = 0;

        if (HAL_OK != HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN))
        {
            break;
        }

        if (sTimeBuffer.tm_wday == 0)
        {
            /*
             * The RTC date structure considers day starts from  monday and
        		ends in sunday and values are in the range1 - 7
        		The time.h library starts from sunday and ends in saturday  and
        		values are in range 0 - 6.
        	 */
        	sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
        }

        sDate.WeekDay   = (uint8_t)sTimeBuffer.tm_wday;
        sDate.Month     = (uint8_t)sTimeBuffer.tm_mon;
        sDate.Date      = (uint8_t)sTimeBuffer.tm_mday;

        /*
         *   In time.h strptime function  returns the year member as no of years
		 	 passed since 1900 but the RTC date structure expects a value from 0 to 99.
         	 So the following calculation is done.
         */

        sDate.Year      = (uint16_t)(sTimeBuffer.tm_year);

        if (HAL_OK != HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN))
        {
            break;
        }

        bRetVal = true;

    } while(0);

    return bRetVal;
}

