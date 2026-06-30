/*
 * Helper.c
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */
#include "DisplayHandler.h"
#include "bitmap.h"


#define SWAPBYTES(c) 			((c << 8) + (c >> 8))

extern _sDeviceInfoData sDeviceInfo;
static uint8_t ucProcessPercentage	= 0;
static eDisplyStatus eDispStatus	= eDisplayError;
static bool blUpdateFlag			= false;
static uint8_t ucBatteryPercentage	= 0;

/* Function Prototypes */
static void UpadteNotConnectedStatus();
static void UpdatePercentage();
static void UpdateDisplayConfigMode();
static void DisplayBleLogo();

/*-----------------------------------------------------------------------------
//Purpose   : Battery percentage getter
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/








uint8_t GetBatteryPercentage()
{
	float Temp = 0;

	//GetBatteryChargePercentage(&Temp);
	ucBatteryPercentage = (uint8_t) Temp;



	if (100 < ucBatteryPercentage)
	{
		ucBatteryPercentage = 100;
	}

	return ucBatteryPercentage;
}

/*-----------------------------------------------------------------------------
//Purpose   : Update BLE logo
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void DisplayBleLogo()
{
	/*SSD1306_GotoXY (1,1);
	SSD1306_Puts (BLE_TEXT, &Font_7x10, 1);*/

	SSD1306_DrawBitmap(75, 1, bluetoothlogo, 10, 13, 1);
	SSD1306_UpdateScreen();


	//if (eReadingStatusUnknown == GetReadingState())
	{
		SSD1306_GotoXY (1,20);
		SSD1306_Puts (SCREENING_READY1, &Font_7x10, 1);
		SSD1306_GotoXY (1,35);
		SSD1306_Puts (SCREENING_READY2, &Font_7x10, 1);

	}

	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update Error code om Display
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void UpdateErrorCodeOnDisplay()
{
	uint8_t ucErrorCode[10] = {'\0'};
	uint16_t unErr = GetDaqErrorStatus();
	unErr = SWAPBYTES(unErr);
	sprintf(ucErrorCode, "%s:%u", ERROR_CODE_TEST,unErr);
	SSD1306_GotoXY (1,1);
	SSD1306_Puts (ucErrorCode, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update Battery voltage on display
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/

/*	void UpdateBatteryVoltageOnDisplay()
	{
		uint8_t x,y;
		 SSD1306_DrawBitmap(99,0, battery, 25, 14, 1);
		 SSD1306_GotoXY (103,4);


		uint8_t batteryString[3] = {'\0'};
		uint8_t battery = GetBatteryPercentage();

		float batV = 0;

		GetBatteryVoltage(&batV);

		if ((battery<100)&&(battery>9)||(battery==0)){
				SSD1306_GotoXY (105,4);
		}
		else if((battery<10)&&(battery>0)) {
		SSD1306_GotoXY (108,4);
		}
		else {
			SSD1306_GotoXY (102,4);

		}


		if ( 0 == battery)
		{
			sprintf(batteryString, "%s","LO");
		}
		else
		{
			sprintf(batteryString, "%d%",battery);
		}



		SSD1306_Puts (batteryString, &Font_6x8, 1);
		SSD1306_UpdateScreen();

	}  */



void UpdateBatteryVoltageOnDisplay() {
    static uint8_t x, y;
    uint8_t batteryLevel = GetBatteryPercentage();
    float batteryVoltage = 0;
    //batteryVoltage=GetBatGaugeCompensation();
    uint8_t batteryString[3] = {'\0'};; // Adjusted buffer size to accommodate 3 digits

    // Set position based on battery level
    if ((batteryLevel < 100 && batteryLevel > 9) || (batteryLevel == 0)) {
        x = 105;
    } else if (batteryLevel < 10 && batteryLevel > 0) {
        x = 108;
    } else {
        x = 102;
    }
    y = 4;




    // Set battery string based on battery level
    if (batteryLevel == 0) {
        sprintf(batteryString, "%s", "LO");
    } else {
        sprintf(batteryString, "%d", batteryLevel);
    }

    // Battery symbol & Clear previous battery percentage
    SSD1306_DrawBitmap(99, 0, battery, 25, 14, 1);
    SSD1306_GotoXY(103, 4);
    SSD1306_Puts("   ", &Font_6x8, 1);


    // Update display with battery percentage
    SSD1306_GotoXY(x, y);
    SSD1306_Puts(batteryString, &Font_6x8, 1);
    SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update BLE logo
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void DisplayDAQ_Id()
{
	uint8_t ucDevId[15] = {'\0'};

	memset(ucDevId, ' ', 14);
	SSD1306_GotoXY (30,50);
	SSD1306_Puts (ucDevId, &Font_7x10, 1);
	SSD1306_UpdateScreen();
	memcpy(ucDevId, sDeviceInfo.ucDeviceId, LENGTH_OF_DEV_ID);
	SSD1306_GotoXY (33,50);
	SSD1306_Puts (ucDevId, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Handle display / Update display
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void DisplayHandler()
{

	eDisplyStatus eStatus = GetDisplayStatus();

	switch (eStatus)
	{
		case eDisplayError:
			SSD1306_Clear();
			break;
		case eDisplayNotConnected:
			UpadteNotConnectedStatus();
			break;
		case eDisplayConnected:
			UpadteConnectedStatus();
			ReadyForScreening();
			break;
		case eDisplayReadingStarted:
			//UpdatePercentage();
			break;
		case eDisplayReadingCompltd:
			break;
		case eDisplayConfigMode:
			UpdateDisplayConfigMode();
			break;
		case eDisplayDisconnected:
			UpdateDisconnectedMsg();
			break;
	}

	UpdateErrorCodeOnDisplay();
	UpdateBatteryVoltageOnDisplay();
	DisplayDAQ_Id();

	blUpdateFlag = false;
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with custom message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
bool UpdateDisplayWithMsg(char* cMessageToDisplay,uint8_t ucLen)
{
	bool blRetVal			= false;
	uint8_t ucFirstMsg1[15]	= {0};
	uint8_t ucFirstMsg2[25]	= {0};

	if (NULL != cMessageToDisplay)
	{
		SSD1306_Clear();
		SSD1306_GotoXY (1,1);

		if (15 < ucLen)
		{
			memcpy(ucFirstMsg1, cMessageToDisplay,15);
			SSD1306_Puts ((char*)ucFirstMsg1, &Font_7x10, 1);
			SSD1306_GotoXY (1, 20);
			memcpy(ucFirstMsg2, cMessageToDisplay + 15, ucLen - 15);
			SSD1306_Puts ((char*)ucFirstMsg2, &Font_7x10, 1);
		}
		else
		{
			SSD1306_Puts (cMessageToDisplay, &Font_11x18, 1);
		}

		SSD1306_UpdateScreen();
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with waiting for connection
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void UpdateDisplayConfigMode()
{
	SSD1306_Clear();
	SSD1306_GotoXY (1,30);
	SSD1306_Puts (CONFIG_MODE, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}
/*-----------------------------------------------------------------------------
//Purpose   : Update display with Disconnected message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void UpdateDisconnectedMsg()
{
	SSD1306_GotoXY (74, 0);
	SSD1306_Puts (BLE_DISCONNECTED, &Font_7x10, 1);

	SSD1306_GotoXY (74, 7);
	SSD1306_Puts (BLE_DISCONNECTED, &Font_7x10, 1);
	//if (eReadingStatusUnknown == GetReadingState())
	{
		ClearNotificationArea();
		SSD1306_GotoXY (1,20);
		SSD1306_Puts (READY_TO_CONNECT, &Font_7x10, 1);
	}

	SSD1306_UpdateScreen();
}
/*-----------------------------------------------------------------------------
//Purpose   : Update display with waiting for connection
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void UpadteNotConnectedStatus()
{
	ClearNotificationArea();
	SSD1306_GotoXY (1,20);
	SSD1306_Puts (READY_TO_CONNECT, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update Reading completed
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void UpadteDaqCompletedStatus()
{
	uint8_t ucUhiIdGet[SUBJECT_UHI_ID_LEN] = {'\0'};
	//ClearNotificationArea(); // Bug: Calling this clears the screen but doesnt display the contents below although it is executing
	SSD1306_GotoXY (2, 20);
	SSD1306_Puts (CLEAR_NOTIF_AREA, &Font_11x18, 1);
	SSD1306_UpdateScreen();
	GetUhiId(ucUhiIdGet);
	ucUhiIdGet[16] ='\0';
	SSD1306_GotoXY (1,20);
	SSD1306_Puts (DAQ_COMPLETED1, &Font_7x10, 1);
	SSD1306_GotoXY (0, 35);
	SSD1306_Puts (" ", &Font_7x10, 1);
	SSD1306_GotoXY (8, 35);
	SSD1306_Puts (ucUhiIdGet, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update memory Reading completed
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void UpadteMemoryReadCompletedStatus()
{
	uint8_t ucUhiIdGet[SUBJECT_UHI_ID_LEN] = {'\0'};
	ClearNotificationArea();
	SSD1306_GotoXY (0, 20);
	SSD1306_Puts (MEMORY_READ_COMPLTD, &Font_7x10, 1);
	SSD1306_GotoXY (1,35);
	GetUhiId(ucUhiIdGet);
	ucUhiIdGet[16] ='\0';
	SSD1306_GotoXY (8, 35);
	SSD1306_Puts (ucUhiIdGet, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with connected status
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void UpadteConnectedStatus()
{
	DisplayBleLogo();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with ready for screening message 
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ReadyForScreening()
{
	ClearNotificationArea();
	SSD1306_GotoXY (1, 20);
	SSD1306_Puts (READY_SCREEING, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with ready for screening message 
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BasicInfoRegistered()
{
	ClearNotificationArea();
	SSD1306_GotoXY (0, 20);
	SSD1306_Puts (BASIC_INFO_REGISTERED, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with basic info downloaded
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BasicInfoDownloaded()
{
	uint8_t ucUhiIdGet[17] = {'\0'};

	ClearNotificationArea();
	SSD1306_GotoXY (0, 20);
	SSD1306_Puts (BASIC_INFO_DOWNLOADED, &Font_7x10, 1);
	GetUhiId(ucUhiIdGet);
	ucUhiIdGet[16] ='\0';
	SSD1306_GotoXY (8, 35);
	SSD1306_Puts (ucUhiIdGet, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

/*-----------------------------------------------------------------------------
//Purpose   : Set or increment the percentage
//Inputs    : blInrimentFlg - to indicate increase %,  ucChangePercentage - amount
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void UpdatePercentage()
{
	char ucDisplayPercentage[6] = { 0, };
	static uint8_t ucPercentage = 0;
	static bool blInitDone = false;

	if (0 == (ucPercentage = GetPercentage()))
	{
		blInitDone = false;
	}

	if((!blInitDone)||(eReadingFromFlash == GetReadingState()))
	{
		ClearNotificationArea();
		SSD1306_GotoXY (1,20);

		if (eReadingFromFlash == GetReadingState())
		{
			SSD1306_Puts (READ_FROM_MEMORY1, &Font_7x10, 1);  /* this part along with percentage is working */
			SSD1306_GotoXY (1,35);
			SSD1306_Puts (READ_FROM_MEMORY2, &Font_7x10, 1);
		}
		else
		{
			/* this one collecting data only appears, percentage is not shown, while debugging 
								it entrs the percentage portion. but doesnt updates on display */
			SSD1306_Puts (COLLECTING_DATA1, &Font_7x10, 1);
			SSD1306_GotoXY (1,35);
			SSD1306_Puts (COLLECTING_DATA2, &Font_7x10, 1);
		}

		SSD1306_UpdateScreen();
		blInitDone = 1;
	}

	sprintf(ucDisplayPercentage,"%d%%  ", ucPercentage);  // percentage display
	SSD1306_GotoXY (80, 20);
	SSD1306_Puts (ucDisplayPercentage, &Font_11x18, 1);
	SSD1306_UpdateScreen();

}

/*-----------------------------------------------------------------------------
//Purpose   : Set or increment the percentage
//Inputs    : blInrimentFlg - to indicate increase %,  ucChangePercentage - amount
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetPercentage(bool blInrimentFlg, uint8_t ucChangePercentage)
{
	if ((100 == ucProcessPercentage) && (true == blInrimentFlg))
	{
		return;
	}

	if (blInrimentFlg)
	{
		ucProcessPercentage = ucChangePercentage;

		if (100 < ucChangePercentage)
		{
			ucProcessPercentage = 100;
			blUpdateFlag = true;
		}
	}
	else if(0 == ucChangePercentage)
	{
		ucProcessPercentage = 0;
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Increment percentage
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void IncreasePercentage(bool bFirstFlag)
{
	/*
	 * Do the calculation here 10 iteration
	 * so each step is 10%
	 */
	if (100 > ucProcessPercentage)
	{
		if (true == bFirstFlag)
		{
			ucProcessPercentage = 0;
		}
		else
		{
			ucProcessPercentage += 10;
		}
		UpdatePercentage();
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Set 100 percentage
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetCompletedDisplayStatus()
{
	ucProcessPercentage = 100;
	blUpdateFlag = true;
	DisplayHandler();
}

/*-----------------------------------------------------------------------------
//Purpose   : Process command
//Inputs    : blInrimentFlg - to indicate increase %,  ucChangePercentage - amount
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint8_t GetPercentage()
{
	return ucProcessPercentage;
}

/*-----------------------------------------------------------------------------
//Purpose   : Process command
//Inputs    : blInrimentFlg - to indicate increase %,  ucChangePercentage - amount
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ClearPercentage()
{
	ucProcessPercentage = 0;
}

/*-----------------------------------------------------------------------------
//Purpose   : Get Display status
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
eDisplyStatus GetDisplayStatus()
{
	return eDispStatus;
}

/*-----------------------------------------------------------------------------
//Purpose   : Set Display status
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetDisplayStatus(eDisplyStatus eStatus)
{
	eDispStatus = eStatus;
	blUpdateFlag = true;
	DisplayHandler();
}

/*-----------------------------------------------------------------------------
//Purpose   : Put welcome Logo on the display
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void C_METwelcomeLogo()
{
	SSD1306_Init();
	HAL_Delay(500);
	SSD1306_GotoXY (10,20);
    SSD1306_Puts (" C-MET ", &Font_16x26, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(3000);
	SSD1306_Clear();


	/*Intelligent IOT Sensors LOGO*/

	SSD1306_DrawBitmap(-4,-10, logo45, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(42);
	SSD1306_Clear();

	SSD1306_DrawBitmap(-4,-10, logo46, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(42);
	SSD1306_Clear();

	SSD1306_DrawBitmap(-4,-10, logo47, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(1000);
	SSD1306_Clear();






	SSD1306_DrawBitmap(-4,-10, logo1, 128, 64, 1);       //IOT LOGO
	SSD1306_UpdateScreen();
						            		    		        HAL_Delay(42);
						            		    		        SSD1306_Clear();

						            		    		        SSD1306_DrawBitmap(-4,-10, logo2, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo3, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();


						            		    		               SSD1306_DrawBitmap(-4,-10 ,logo4, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();


						            		    		               SSD1306_DrawBitmap(-4,-10, logo5, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10 ,logo6, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo7, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo8, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo9, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10 ,logo10, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo11, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo12, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo13, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo14, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(42);
						            		    		               SSD1306_Clear();

						            		    		               SSD1306_DrawBitmap(-4,-10, logo15, 128, 64, 1);
						            		    		               SSD1306_UpdateScreen();
						            		    		               HAL_Delay(500);




						            				               SSD1306_DrawBitmap(-4,-10, logo29, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);



						            				               SSD1306_GotoXY (0,48);
						            				               SSD1306_Puts ("    THINK GLOBALLY   ", &Font_6x8, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(1000);


						            				              				            				               SSD1306_GotoXY (0,48);
						            				              				            				               SSD1306_Puts ("     MAKE LOCALLY    ", &Font_6x8, 1);
						            				              				            				               SSD1306_UpdateScreen();
						            				              				            				               HAL_Delay(1000);
						            				              				            				               SSD1306_Clear();






						            				               SSD1306_DrawBitmap(-4,-10, logo30, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo31, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo32, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo33, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo34, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo35, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo36, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo37, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo38, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo39, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo40, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo41, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo42, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo43, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);
						            				               SSD1306_Clear();


						            				               SSD1306_DrawBitmap(-4,-10, logo44, 128, 64, 1);
						            				               SSD1306_UpdateScreen();
						            				               HAL_Delay(42);




}

/*-----------------------------------------------------------------------------
//Purpose   : Put device info on stratup
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void DeviceInfoLogo()
{
	uint8_t ucCustomPrint[20]	= {'\0'};
	uint8_t ucDevId[15]			= {'\0'};
	SSD1306_GotoXY (5,10);
	memset(ucCustomPrint, 0x00, 12);
	memcpy(ucDevId, sDeviceInfo.ucWearableId, LENGTH_OF_DEV_ID);
	sprintf(ucCustomPrint, "%s: %s", "WD ID", ucDevId);
	SSD1306_Puts (ucCustomPrint, &Font_7x10, 1);
	SSD1306_GotoXY (5,30);
	memset(ucCustomPrint, 0x00, 12);
	memcpy(ucDevId, sDeviceInfo.ucDeviceId, LENGTH_OF_DEV_ID);
	sprintf(ucCustomPrint, "%s: %s", "MD ID", ucDevId);
	SSD1306_Puts (ucCustomPrint, &Font_7x10, 1);

	SSD1306_GotoXY (5,40);
	sprintf(ucCustomPrint, "%s: %s", "HW Ver", HW_VERSION);
	SSD1306_Puts (ucCustomPrint, &Font_7x10, 1);

	SSD1306_GotoXY (5,50);
	memset(ucCustomPrint, 0x00, 12);
	sprintf(ucCustomPrint, "%s: %s", "FW Ver", SW_VERSION);
	SSD1306_Puts (ucCustomPrint, &Font_7x10, 1);

	SSD1306_UpdateScreen();
	HAL_Delay(5000);
	SSD1306_Clear ();
}


/*-----------------------------------------------------------------------------
//Purpose   : Clearing the notification area of the display
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ClearNotificationArea()
{
	SSD1306_GotoXY (1,20);
	SSD1306_Puts (CLEAR_NOTIF_AREA, &Font_11x18, 1);  /* this part along with percentage is working */
	SSD1306_GotoXY (1,35);
	SSD1306_Puts (CLEAR_NOTIF_AREA, &Font_7x10, 1);  /* this part along with percentage is working */
	SSD1306_UpdateScreen();
}


void UpdateRestartDevice()
{
	SSD1306_Clear();
	SSD1306_GotoXY (1,20);
	SSD1306_Puts (RESTART1, &Font_7x10, 1);
	SSD1306_GotoXY (1,30);
	SSD1306_Puts (RESTART2, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}
