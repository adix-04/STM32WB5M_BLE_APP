/*
 * bleDb.h
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */
#ifndef BLE_DB
#define BLE_DB

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "app_conf.h"
#include "stm32_seq.h"
#include "DisplayHandler.h"
#include "PacketHandler.h"

#define MAX_BLE_MSG		512


typedef struct __sBleDataBase
{
	bool 		blIsDataFilled;
	uint8_t		ucPayload[MAX_BLE_MSG];
	uint16_t	unLength;
}_sBleDataBase;

typedef enum __eBleDeviceStatus
{
	eDeviceStateUnknown 		= 0,
	eDeviceStateNotConnected	= 1,
	eDeviceStateConnected		= 2
}_eBleDeviceStatus;

typedef enum __eBleRequestStatus
{
	eBleStatusError 		= 0,
	eBleRequestReceived 	= 1,
	eBleSendResOrData		= 2,
}_eBleRequestStatus;

typedef enum __eReadingFrom
{
	eRedaingNotSet		= 0,
	eReadingFromFlash 	= 1,
	eLiveReading		= 2
}_eReadingFrom;

bool PushToBleTxDataBase(void* ucPayload, uint16_t unLength);
void ClearBleTxDataBase();
uint8_t* PopFromBleTxDataBase();
uint16_t GetBleTxDataBaseMessageLen();
void BleDataSetReadingSource(_eReadingFrom eSource);
bool SendMessageToBle(void* ucPayload, uint16_t ucLength);
void ClearBleTxDataBase();
void ClearBleRxDataBase();
bool BleRxDataBaseIsFree();
void BleDataSetRequestStatus(_eBleRequestStatus eSource);
_eBleRequestStatus GetBleDataRequestStatus();
bool BleTxDataBaseStatus();
bool BleRxDataBaseStatus();
uint16_t GetBleRxDataBaseMessageLen();
uint8_t* PopFromBleRxDataBase();
_eReadingFrom GetReadingState(void);
void ProcessReadingFromFlash();
void SetRedaingState(_eReadingFrom eState);
void SendBleNotification();
bool PushToBleRxDataBase(void* ucPayload, uint16_t unLength);

#endif
