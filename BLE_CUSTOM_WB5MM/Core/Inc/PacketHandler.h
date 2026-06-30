/*
 * PacketHandler.h
 *
 *  Created on: Dec 8, 2022
 *      Author: 0000
 */

#ifndef PACKET_HANDLER
#define PACKET_HANDLER

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_RESPONSE_LEN		5
#define PAYLOAD_PACKET_LEN		9
#define NAME_LEN				50
#define AGE_LEN					3
#define DAQ_ID_LEN				10
#define SUBJECT_UHI_ID_LEN		16
#define SUBJECT_WD_SIZE_LEN		4
#define SUBJECT_WD_ID_LEN		10
#define ROOM_TEMP_LEN			7
#define BODY_TEMP_LEN			7
#define TIME_STAMP_LEN			12
#define TEMP_PAYLOAD_SIZE		48

typedef enum
{
	eAppInitRequest			= 0,
	eSetConfigDetails		= 1,
	eStartScreening 		= 2,
	eGetConfigDetails		= 4,
	eDownloadScreeningData 	= 5,
	eBuzzerRequest			= 6,
}eRequestType;

typedef enum
{
	eAppInitResp			= 0,
	eSetConfigResp			= 1,
	eStartScreeningResp		= 2,
	eLiveScreeningPayload	= 3,
	eGetConfigDetailsResp	= 4,
	eScreeningDataPayload 	= 5,
	eBuzzerResponce			= 6,
}eResponseType;

typedef enum __eErrorCodes
{
	STATUS_OK,
	COMMAND_ERROR,
	PER_COMM_ERROR,
	DAQ_CHANNEL_ERROR,
	SET_CONFIG_FAILED,
	DEVICE_ID_ABSENT,
	WEARABLE_ID_ABSENT,
	UHI_ID_ABSENT,
	SENSOR_ERROR,
	FLASH_WRITE_ERROR,
	FLASH_READ_ERROR,
	LOW_BATTERY_ERROR
}_eErrorCodes;

typedef enum __eErrorCode
{
	eErrorNone					= 0,
	eErrorBatLOw				= 1 << 0,
	eErrorDaqIdMisMatch			= 1 << 1,
	eErrorWearableIdMisMatch	= 1 << 2,
	eErrorUhiIdMisMatch			= 1 << 3,
	eErrorWearableSizeMisMatch	= 1 << 4,
	eErrorMax					= 0XFFFF
}_eErrorCode;

typedef struct
{
	uint8_t 	ucReq_type;
	uint8_t		ucPacketLen;
	uint8_t		ucInitCmd;
	uint8_t		ucTimeStamp[TIME_STAMP_LEN];
	uint8_t		unCheckSum;
}sAppInitRequest;

typedef struct
{
	uint8_t 	ucReq_type;
	uint8_t		ucPacketLen;
	uint16_t	ucErrorCode;
	uint8_t		ucDaqId[DAQ_ID_LEN];
	uint8_t		ucSubjectWdSize[SUBJECT_WD_SIZE_LEN];
	uint8_t		ucSubjectWdId[SUBJECT_WD_ID_LEN];
	uint8_t		ucBatteryPercentage;
	uint8_t		unCheckSum;
}sAppInitRespMsg;

typedef struct
{
	uint8_t 	ucReq_type;
	uint8_t		ucPacketLen;
	uint8_t		ucStartScreen;
	uint8_t		ucTimeStamp[TIME_STAMP_LEN];
	uint8_t		unCheckSum;
}sStartScreeningSMsg;

typedef struct __attribute__((packed))
{
	uint8_t 		ucReq_type;
	uint8_t			ucPacketLen;
	uint16_t		ucErrorCode;
	uint8_t			ucStatus;
	uint8_t			ucBatteryPercentage;
	uint8_t			unCheckSum;
}sStartScreeningResponceSMsg;

typedef struct
{
	uint8_t 	ucReq_type;
	uint8_t		ucPacketLen;
	uint8_t		ucDownloadScreenData;
	uint8_t 	ucSubjectUhiId[SUBJECT_UHI_ID_LEN];
	uint8_t		ucPacketType;
	uint8_t		unCheckSum;
}sDownloadScreeningData;

/* Currently not being used */
typedef struct __attribute__((packed))
{
	uint8_t 		ucReq_type;
	uint8_t			ucPacketLen;
	_eErrorCode 	sErrorCode;
	uint8_t			ucStatus;
	uint8_t			ucDataSetIndx;
	uint8_t			ucTimeStamp[TIME_STAMP_LEN];
	float 			ucPayload[TEMP_PAYLOAD_SIZE];
	uint8_t			ucValidationStatus;
	uint8_t			ucBatteryPercentage;
	uint8_t			ucCheckSum;
}sDownloadScreeningDataResp;

typedef struct __attribute__((packed))
{
	uint8_t 		ucReq_type;
	uint8_t			ucPacketLen;
	_eErrorCode 	sErrorCode;
	uint8_t			ucStatus;
	uint8_t			ucDataSetIndx;
	uint8_t			ucTimeStamp[TIME_STAMP_LEN];
	float 			ucPayload[TEMP_PAYLOAD_SIZE];
	uint8_t			ucValidationStatus;
	uint8_t			ucBatteryPercentage;
	uint8_t			ucCheckSum;
}sScreeningData;

typedef enum __eErrorCode_Bits
{
	eStatus_ok 					= 0,
	eCommand_Error 				= 1 << 0,
	ePer_Com_Error 				= 1 << 1,
	eDaq_Channel_Error 			= 1 << 2,
	eSet_Confog_Failed 			= 1 << 3,
	eDevice_Id_Absent 			= 1 << 4,
	eWearable_Id_Absemnt 		= 1 << 5,
	eUhi_Id_Absent				= 1 << 6,
	eSensor_Error				= 1 << 7,
	eFlash_Write_error			= 1 << 8,
	eFlash_Read_error			= 1 << 9,
	eFlash_Read_Error			= 1 << 10,
	eFlash_Low_Battery_Error 	= 1 << 11
}_eErrorCode_Bits;

typedef struct __sSetConfigFlashData
{
	uint8_t		ucSubjectName[NAME_LEN];
	uint8_t		ucSubjectAge[AGE_LEN];
	uint8_t 	ucSubjectUhiId[SUBJECT_UHI_ID_LEN];
	uint8_t		ucDaqId[DAQ_ID_LEN];
	uint8_t		ucSubjectWdSize[SUBJECT_WD_SIZE_LEN];
	uint8_t		ucSubjectWdId[SUBJECT_WD_ID_LEN];
	uint8_t		ucRoomTemp[ROOM_TEMP_LEN];
	uint8_t		ucBodyTemp[BODY_TEMP_LEN];
	uint8_t		ucTimeStamp[TIME_STAMP_LEN];
}_sSetConfigFlashData;

typedef struct __sSetConfig
{
	uint8_t 	            ucReq_type;
	uint8_t	            	ucPacketLen;
    _sSetConfigFlashData    sSetConfData;
	uint8_t		            ucCheckSum;
}_sSetConfig;

typedef struct __attribute__((packed))
{
	uint8_t 		ucReq_type;
	uint8_t			ucPacketLen;
	_eErrorCode		sErrorCode;
	uint8_t			ucStatus;
	uint8_t			ucBatteryPercentage;
	uint8_t			ucCheckSum;
}_sSetConfigResp;

typedef struct __sGetConfig
{
	uint8_t 		ucReq_type;
	uint8_t			ucPacketLen;
	uint8_t			ucRequest;
	uint8_t 		ucSubjectUhiId[SUBJECT_UHI_ID_LEN];
	uint8_t			ucPacketType;
	uint8_t			ucCheckSum;
}_sGetConfig;

typedef struct __attribute__((packed))
{
	uint8_t 				ucReq_type;
	uint8_t					ucPacketLen;
	_eErrorCode				sErrorCode;
	uint8_t					ucStatus;
	_sSetConfigFlashData    sSavedConfData;
	uint8_t					ucBatteryPercentage;
	uint8_t					ucCheckSum;
}_sGetConfigResp;

typedef enum __eConfigurationDataStatus
{
	eConfigDataStatusUnknown 		= 0,
	eConfigDataStatusLoadedFrmFlash = 1
}_eConfigurationDataStatus;

typedef struct __attribute__((packed))
{
	uint8_t 				ucReq_type;
	uint8_t					ucPacketLen;
	uint8_t					ucAlarm;
	uint8_t					ucTimeStamp[TIME_STAMP_LEN];
	uint8_t					ucCheckSum;
}_sBuzzerReqCmnd;

typedef struct __attribute__((packed))
{
	uint8_t 				ucReq_type;
	uint8_t					ucPacketLen;
	_eErrorCode				sErrorCode;
	uint8_t					ucDaqId[DAQ_ID_LEN];
	uint8_t					ucBatteryPercentage;
	uint8_t					ucCheckSum;
}_sBuzzerResp;

void  ProtocolBuilder(eResponseType eReqType);
void BlePacketHandler();
void StartBuildingScreeningPayload();
void SetDaqErrorStatus(_eErrorCode eErrorStatus);
void ClearDaqErrorStatus(_eErrorCode eErrorStatus);
_eErrorCode GetDaqErrorStatus( void);
void GetUhiId(uint8_t *ucUhiIdGet);
void RestoreConfigDataFromFlash();
void ClearDataSet();

#endif
