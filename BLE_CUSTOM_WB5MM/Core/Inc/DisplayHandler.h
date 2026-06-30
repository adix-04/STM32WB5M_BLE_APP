/*
 * DisplayHandler.h
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */

#ifndef DISPLAY_HANDLER
#define DISPLAY_HANDLER

#include "stdbool.h"
#include <stdlib.h>
#include <stdint.h>
#include "ssd1306.h"
#include "fonts.h"
#include "bleDB.h"
#include "main.h"
#include "PacketHandler.h"

#define WAITING_MSG					"    Waiting For"
#define CONNECTION_MSG				"   Connection "
#define READY_TO_CONNECT			" Ready To Connect "
#define READING_STARTED				"Reading Started"
#define COLLECTING_DATA1			"Screening "
#define COLLECTING_DATA2			"Started"
#define PERCENTAGE_MSG				"Percentage"
#define DAQ_COMPLETED1				"  Screening Done "
#define DAQ_COMPLETED2				"    Completed     "
#define CONNECTED_MSG				"  BLE CONNECTED   "
#define BLE_DISCONNECTED			"   "
#define CONFIG_MODE					"    CONFIG MODE   "
#define FILE_RX_COMPLETED			"  File transfer   completed  "
#define READ_FROM_MEMORY1			"Downloading "
#define READ_FROM_MEMORY2			"Scrn Data"
#define MEMORY_READ_COMPLTD 		"   SD Downloaded  "
#define READY_SCREEING				"Ready for Screeing "
#define BASIC_INFO_REGISTERED		"   BI Registered  "
#define BASIC_INFO_DOWNLOADED		"   BI Downloaded  "
#define SCREENING_READY1				"Ready for         "
#define SCREENING_READY2				"        Screening "
#define BLE_TEXT					"BLE"
#define ERROR_CODE_TEST				"Err"
#define CLEAR_NOTIF_AREA			"                  "
#define RESTART1			"  Please restart  "
#define RESTART2			"    the device    "

typedef enum
{
	eDisplayError 			= 0,
	eDisplayNotConnected 	= 1,
	eDisplayConnected		= 2,
	eDisplayReadingStarted	= 3,
	eDisplayReadingCompltd	= 4,
	eDisplayConfigMode		= 5,
	eDisplayDisconnected	= 6
}eDisplyStatus;

eDisplyStatus GetDisplayStatus();
void SetDisplayStatus(eDisplyStatus eStatus);
void DisplayHandler();
void SetPercentage(bool blInrimentFlg, uint8_t ucChangePercentage);
uint8_t GetPercentage();
bool UpdateDisplayWithMsg(char* cMessageToDisplay,uint8_t ucLen);
void C_METwelcomeLogo();
void IncreasePercentage(bool bFirstFlag);
void UpadteDaqCompletedStatus();
void UpadteMemoryReadCompletedStatus();
void UpdateDisconnectedMsg();
void UpadteConnectedStatus();
void DeviceInfoLogo();
void BasicInfoRegistered();
void BasicInfoDownloaded();
void DisplayDAQ_Id();
void ClearPercentage();
void UpdateRestartDevice();
void UpdateErrorCodeOnDisplay();
void UpdateBatteryVoltageOnDisplay();
uint8_t GetBatteryPercentage();
void ClearNotificationArea();
void ReadyForScreening();

#endif
