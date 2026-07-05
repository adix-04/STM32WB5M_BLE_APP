/*
 * bleDb.c
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */
#include "bleDB.h"
#include "FlashMemory.h"
#include "DisplayHandler.h"
#include "main.h"

static _sBleDataBase sBleTxDB				= {0};
static _sBleDataBase sBleRxDB				= {0};
RTC_TimeTypeDef sReadTime					= {0};
static _eReadingFrom eBleReadingSource;
static _eBleRequestStatus eBleReqStatus 	= eBleStatusError;
static _eReadingFrom eRedingData;

static _eReadingFrom GetBleDataReadingSource();
extern void my_Task(void);
//extern uint* UpdateCharData;
extern float FINAL_TEMP[48];

/*-----------------------------------------------------------------------------
//Purpose   : Push data to ble Tx data base
//Inputs    : ucPayload - data, unLength - length of the data
//Outputs   : None
//Return    : bool - result
//Notes     : None
-----------------------------------------------------------------------------*/
bool PushToBleTxDataBase(void* ucPayload, uint16_t unLength)
{
	bool blRetVal = false;

	if ((0 != unLength) && (NULL != ucPayload) && (false == sBleTxDB.blIsDataFilled))
	{
		ClearBleTxDataBase();
		memcpy(sBleTxDB.ucPayload, (uint8_t*)ucPayload, unLength);
		sBleTxDB.blIsDataFilled = true;
		sBleTxDB.unLength = unLength;
		blRetVal = true;
	}
	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Push data to ble Rx data base
//Inputs    : ucPayload - data, unLength - length of the data	
//Outputs   : None
//Return    : bool - result
//Notes     : None
-----------------------------------------------------------------------------*/
bool PushToBleRxDataBase(void* ucPayload, uint16_t unLength)
{
	bool blRetVal = false;

	if ((0 != unLength) && (NULL != ucPayload) && (false == sBleTxDB.blIsDataFilled))
	{
		ClearBleRxDataBase(); 
		memcpy(sBleRxDB.ucPayload, (uint8_t*)ucPayload, unLength);
		sBleRxDB.blIsDataFilled = true;
		sBleRxDB.unLength = unLength;
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : To clear Ble Tx data base
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ClearBleTxDataBase()
{
	memset(&sBleTxDB, 0x00, sizeof(sBleTxDB));
}

/*-----------------------------------------------------------------------------
//Purpose   : To clear Ble Rx data basae
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ClearBleRxDataBase()
{
	memset(&sBleRxDB, 0x00, sizeof(sBleRxDB));
}

/*-----------------------------------------------------------------------------
//Purpose   : Pop from the Ble Tx data base
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint8_t* PopFromBleTxDataBase()
{
	return &sBleTxDB.ucPayload[0];
}

/*-----------------------------------------------------------------------------
//Purpose   : Pop from Ble Rc data base
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint8_t* PopFromBleRxDataBase()
{
	return &sBleRxDB.ucPayload[0];
}
/*-----------------------------------------------------------------------------
//Purpose   : get the length of the data in ble Tx data base
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint16_t GetBleTxDataBaseMessageLen()
{
	return sBleTxDB.unLength;
}

/*-----------------------------------------------------------------------------
//Purpose   : get the length of the data in ble Tx data base
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint16_t GetBleRxDataBaseMessageLen()
{
	return sBleRxDB.unLength;
}

/*-----------------------------------------------------------------------------
//Purpose   : To check the status of ble Tx data base
//Inputs    : None
//Outputs   : None
//Return    : true - filled, false - free
//Notes     : None
-----------------------------------------------------------------------------*/
bool BleTxDataBaseStatus()
{
	bool blRetVal = false;

	if ((0 < sBleTxDB.unLength) || (sBleTxDB.blIsDataFilled))
	{
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : To check the status of ble Rx data base
//Inputs    : None
//Outputs   : None
//Return    : true - filled, false - free
//Notes     : None
-----------------------------------------------------------------------------*/
bool BleRxDataBaseStatus()
{
	bool blRetVal = false;

	if ((0 < sBleRxDB.unLength) || (sBleRxDB.blIsDataFilled))
	{
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with custom message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
bool SendMessageToBle(void* ucPayload, uint16_t ucLength)
{
	bool blRetVal = false;

	if (!BleTxDataBaseStatus())
	{
		blRetVal = PushToBleTxDataBase(ucPayload, ucLength);
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Send Readings pay load
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SendReadingsToBle()
{
	static int unIterator = 0;
	uint8_t*	ucBuffer = NULL;

	if (48 > unIterator)
	{
		switch(GetBleDataReadingSource())
		{
			case eReadingFromFlash:
				//ucBuffer = GetStoredReadingsFromFlash();
				if (true == SendMessageToBle(ucBuffer, 32))
				{
					unIterator++;
				}
				break;

			case eLiveReading:
				//ucBuffer =
			default:
				break;
		}
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : To set the Ble payload source Flash / live data
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BleDataSetReadingSource(_eReadingFrom eSource)
{
	eBleReadingSource = eSource;
}

/*-----------------------------------------------------------------------------
//Purpose   : To Get the Ble payload source Flash / live data
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static _eReadingFrom GetBleDataReadingSource()
{
	return eBleReadingSource;
}

/*-----------------------------------------------------------------------------
//Purpose   : To set the Ble payload source Flash / live data
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BleDataSetRequestStatus(_eBleRequestStatus eSource)
{
	eBleReqStatus = eSource;
}

/*-----------------------------------------------------------------------------
//Purpose   : To Get the Ble payload source Flash / live data
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
_eBleRequestStatus GetBleDataRequestStatus()
{
	return eBleReqStatus;
}

/*-----------------------------------------------------------------------------
//Purpose   : Set Reading Source
//Inputs    : eRedingData - source
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetRedaingState(_eReadingFrom eState)
{
	eRedingData = eState;
}

/*-----------------------------------------------------------------------------
//Purpose   : Get Reading Source
//Inputs    : None
//Outputs   : None
//Return    : eRedingData - source
//Notes     : None
-----------------------------------------------------------------------------*/
_eReadingFrom GetReadingState(void)
{
	return eRedingData;
}

/*-----------------------------------------------------------------------------
//Purpose   : Process ReadingState from flash
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ProcessReadingFromFlash()
{
	static uint8_t ucReadingItr = 0;

	if (MAX_READING_ITERATION > ++ucReadingItr)
	{
		memset(FINAL_TEMP, 0, sizeof(FINAL_TEMP));
		memset(&sReadTime, 0x00, sizeof(RTC_TimeTypeDef));
		ProtocolBuilder(eScreeningDataPayload);

		if(0 == ucReadingItr)
		{
			IncreasePercentage(true);
		}
		else
		{
			IncreasePercentage(false);
		}
	}
	else
	{
		//BuzzerPlay(1);
		UpadteMemoryReadCompletedStatus();
		HAL_Delay(100);
		SetRedaingState(eRedaingNotSet);
		ucReadingItr = 0;
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : BLE send Notification data 
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SendBleNotification()
{
	UTIL_SEQ_SetTask(1 << CFG_TASK_my_task, CFG_SCH_PRIO_0);/* Push the ble related function to Scheduler */
}
