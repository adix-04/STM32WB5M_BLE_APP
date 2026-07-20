/*
 * PacketHandler.c
 *
 *  Created on: Mar 8, 2023
 *      Author: Alex
 */
#include "crc.h"
#include "PacketHandler.h"
#include "bleDB.h"
#include "main.h"
#include "RTCC.h"
#include "FlashMemory.h"
#include "DisplayHandler.h"
#include <string.h>
#include <stdint.h>

#define SWAPBYTES(c) ( (c << 8) + (c >> 8))

extern float FINAL_TEMP[48];
extern _sDeviceInfoData sDeviceInfo;

char* ucpWearableSize[] =  {
		"S",
		"M",
		"L",
		"XL",
		"XXL",
		"XXXL",
		"FULL"
};

/* Global Declarations */
static bool StartScreeningResponseBuilder();
static bool ScreeningCompltdMsgBuilder();
static bool ScreeningDataPayloadBuilder();
static void ProtocolParser(uint8_t* ucPayLoad, uint16_t ucMsgLen);
static bool HandleDownloadScreeningDataPckt(uint8_t* ucDownloadPackt, uint16_t usDataLen);
static bool HandleStartScreeingDataPckt(uint8_t* ucStartScrnPck, uint16_t usDataLen);
static bool BuildSetConfigurationResponse();
static bool HandleSetConfigDetailsPckt(uint8_t* ucConfigPacket, uint16_t usDataLen);
static bool HandleGetSavedConfigurationDetails(uint8_t* ucConfigGetPacket, uint16_t usDataLen);
static bool HandleAppInitMessagePckt(uint8_t* ucConfigPacket, uint16_t usDataLen);
static bool HandleBuzzerPlayRequest(uint8_t* ucConfigPacket, uint16_t usDataLen);
static bool BuildBuzzerResponce();
static bool BuildAppInitMessageResponse();
static bool BuildGetConfigDetailsResponse();
static void ClearAllErrorStatus();

static _sSetConfigFlashData sConfigurationData		= {0};
static _eErrorCode eDaqErrorStatus 					= eErrorNone;
bool configflag 									= false;

/*-----------------------------------------------------------------------------
//Purpose   : Restore flash data
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void RestoreConfigDataFromFlash()
{
	FlashReadConfigurationData((uint8_t *)&sConfigurationData);
}

/*-----------------------------------------------------------------------------
//Purpose   : Ble communication packet manager
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void GetUhiId(uint8_t *ucUhiIdGet)
{
	uint8_t ucUhiIdGetCopy[16]={'\0'};

	if ((ucUhiIdGet) && (sConfigurationData.ucSubjectUhiId))
	{
		memcpy(ucUhiIdGetCopy,sConfigurationData.ucSubjectUhiId, sizeof(sConfigurationData.ucSubjectUhiId));
		sprintf(ucUhiIdGet, "%s",ucUhiIdGetCopy);
	}
}
/*-----------------------------------------------------------------------------
//Purpose   : Ble communication packet manager
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void BlePacketHandler()
{
	_eBleRequestStatus eStatus;
	uint8_t * ucpReqData = PopFromBleTxDataBase();

	eStatus = GetBleDataRequestStatus();
//
	if ((eReadingFromFlash == GetReadingStatus())||(((*ucpReqData) != '3') &&
				(eReadingStatusReadingInProgrs == GetReadingStatus())))
	{
		return;
	}

	switch(eStatus)
	{
		case eBleRequestReceived:
			if (BleRxDataBaseStatus())
			{
				ProtocolParser((uint8_t*)PopFromBleRxDataBase(), GetBleRxDataBaseMessageLen());
			}

			if (eBleSendResOrData != GetBleDataRequestStatus())
				break;

		case eBleSendResOrData:
			if (BleTxDataBaseStatus())
			{
				SendBleNotification();
			}

			break;
		case eBleStatusError:
		default:
			break;
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with custom message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ProtocolBuilder(eResponseType eReqType)
{
	bool blRetVal = false;

	switch(eReqType)
	{
	case eAppInitResp:
		blRetVal = BuildAppInitMessageResponse();
		break;
	case eSetConfigResp:
		blRetVal = BuildSetConfigurationResponse();
		break;
	case eStartScreeningResp:
		blRetVal = StartScreeningResponseBuilder();
		break;
	case eLiveScreeningPayload:
		blRetVal = ScreeningCompltdMsgBuilder();
		break;
	case eScreeningDataPayload:
		blRetVal = ScreeningDataPayloadBuilder();
		break;
	case eGetConfigDetailsResp:
		blRetVal = BuildGetConfigDetailsResponse();
		break;
	case eBuzzerResponce:
		blRetVal = BuildBuzzerResponce();
		break;
	default:
		break;
	}

	if (blRetVal)
	{
		BleDataSetRequestStatus(eBleSendResOrData);
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Update display with custom message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static void ProtocolParser(uint8_t* ucPayLoad, uint16_t ucMsgLen)
{
	uint8_t* ucReq = 0;
	bool blRetVal = false;
	uint8_t ucReqType = 0;

	if(!ucPayLoad)
	{
		return;
	}

	ucReqType = (uint8_t)*ucPayLoad;
	ucReqType = ucReqType - 48;

	switch(ucReqType)
	{
	case eAppInitRequest:
		if(blRetVal = HandleAppInitMessagePckt((uint8_t*)ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eAppInitResp);
		}
		break;

	case eSetConfigDetails:
		if(blRetVal = HandleSetConfigDetailsPckt((uint8_t*)ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eSetConfigResp);
		}
		break;
	case eStartScreening:
		if (blRetVal = HandleStartScreeingDataPckt(ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eStartScreeningResp);
		}
	break;
	case eGetConfigDetails:
		if (blRetVal = HandleGetSavedConfigurationDetails(ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eGetConfigDetailsResp);
		}
	break;
	case eDownloadScreeningData:
		if (blRetVal = HandleDownloadScreeningDataPckt(ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eScreeningDataPayload);
		}
	break;
	case eBuzzerRequest:
		if(blRetVal = HandleBuzzerPlayRequest(ucPayLoad, ucMsgLen))
		{
			ClearBleRxDataBase();
			ProtocolBuilder(eBuzzerResponce);
		}
	default:
		break;
	}

	if(!blRetVal)
	{
		BleDataSetRequestStatus(eBleStatusError);
		ClearBleRxDataBase();
	}
	else
	{
		//UpdateErrorCodeOnDisplay();
		//ClearAllErrorStatus();
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : Build screen message payload
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void StartBuildingScreeningPayload()
{
	ProtocolBuilder(eScreeningDataPayload);
}
/*-----------------------------------------------------------------------------
//Purpose   : Update display with custom message
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static bool StartScreeningResponseBuilder()
{
	sStartScreeningResponceSMsg sScreenMesg = {0};
	bool blRetVal							= false;

	sScreenMesg.ucReq_type 				= eStartScreeningResp + '0';
	sScreenMesg.ucStatus				= 'S';
	sScreenMesg.ucPacketLen 			= sizeof(sScreenMesg);
	sScreenMesg.ucBatteryPercentage 	= GetBatteryPercentage();

	sScreenMesg.unCheckSum =calculateCRC((unsigned char*) &sScreenMesg, sizeof(sScreenMesg) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sScreenMesg, sizeof(sScreenMesg));
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
static bool ScreeningCompltdMsgBuilder()
{
#if 0
	sScreeningCmompleted eScreenCmpltdMsg = {0};
	uint8_t ucCrc = 0;
	bool blRetVal = false;
	eScreenCmpltdMsg.ucReq_type		= eScreeningCompltd + '0';
	eScreenCmpltdMsg.ucSeqNumber	= 0X01;
	eScreenCmpltdMsg.ucErrorCode	= GetDaqErrorStatus();
	eScreenCmpltdMsg.ucStatus		= 'F';
	eScreenCmpltdMsg.unCheckSum = calculateCRC((unsigned char* )&eScreenCmpltdMsg, sizeof(eScreenCmpltdMsg) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&eScreenCmpltdMsg, sizeof(eScreenCmpltdMsg));
		blRetVal = true;
	}

	return blRetVal;
#endif
}

static uint8_t unDefaultValue 			= 0;

void ClearDataSet()
{
	unDefaultValue = 0;
}

/*-----------------------------------------------------------------------------
//Purpose   : Building Pay load packets to Ble
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
static bool ScreeningDataPayloadBuilder()
{
	sScreeningData sCreenDataPayload 		= {0};
	bool blRetVal 							= false;
	uint8_t sTimeStamp[6] 					= {0};
	RTC_DateTypeDef sCurrentDate 			= {0};
	RTC_TimeTypeDef sCurrentTime   			= {0};

	sCreenDataPayload.ucReq_type 			= eScreeningDataPayload + '0';
	sCreenDataPayload.ucPacketLen 			= sizeof(sCreenDataPayload);
	sCreenDataPayload.sErrorCode 			= GetDaqErrorStatus();
	sCreenDataPayload.ucStatus 				= 'S';
	sCreenDataPayload.ucValidationStatus	= CheckValidityOfLastRead();
	sCreenDataPayload.ucDataSetIndx 		= unDefaultValue;
	sCreenDataPayload.ucBatteryPercentage 	= GetBatteryPercentage();

	if (unDefaultValue < (MAX_READING_ITERATION - 1))
	{
		unDefaultValue++;
	}
	else
	{
		unDefaultValue = 0;
	}

	if (eLiveReading != GetReadingState())
	{
		FlashReadPatientResultData(FINAL_TEMP, (uint8_t)sCreenDataPayload.ucPayload, sTimeStamp);
		/* Convert time stamp to the receptor format */
		sprintf((char*)sCreenDataPayload.ucTimeStamp,"%02d%02d%02d%02d%02d%02d",
												sTimeStamp[0], sTimeStamp[1], sTimeStamp[2],
				sTimeStamp[3], sTimeStamp[4], sTimeStamp[5]);
	}
	else
	{
		sCreenDataPayload.ucReq_type 			= eLiveScreeningPayload + '0';
//
		if (GetRtcTime(&sCurrentTime) && GetRtcDate(&sCurrentDate))
		{
			/* Convert time stamp to the receptor format */
			sprintf(sCreenDataPayload.ucTimeStamp,"%02d%02d%02d%02d%02d%02d",
					sCurrentDate.Date, sCurrentDate.Month, sCurrentDate.Year,
				sCurrentTime.Hours, sCurrentTime.Minutes, sCurrentTime.Seconds);
		}
		else
		{
			// Set error code here
			return blRetVal;
		}
	}

	memcpy(sCreenDataPayload.ucPayload, FINAL_TEMP,sizeof(FINAL_TEMP) );

	//sCreenDataPayload.ucCheckSum = calculateCRC((unsigned char*)&sCreenDataPayload,  (PAYLOAD_PACKET_LEN + sizeof(FINAL_TEMP) - 1));
	sCreenDataPayload.ucCheckSum = calculateCRC((unsigned char*)&sCreenDataPayload,  (sizeof(sCreenDataPayload) - 1));

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sCreenDataPayload, PAYLOAD_PACKET_LEN + sizeof(sCreenDataPayload));
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Start screening Message
//Inputs    : ucStartScrnPck - screening Packet, usDataLen - lengtyh of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool HandleStartScreeingDataPckt(uint8_t* ucStartScrnPck, uint16_t usDataLen)
{
	bool vlRetVal = false;

	if(checkCRC(ucStartScrnPck, usDataLen))
	{
		IncreasePercentage(true);
		ProcessDataAquesitionRequest();
		SetReadingStatus(eReadingStatusReadingInProgrs);
		SetRedaingState(eLiveReading);
		ClearDataSet();
		vlRetVal = true;
	}

	return vlRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Start screening Message
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
bool HandleDownloadScreeningDataPckt(uint8_t* ucDownloadPackt, uint16_t usDataLen)
{
	bool vlRetVal 						= false;
	sDownloadScreeningData usDownldDta 	= { 0 };

	if(checkCRC(ucDownloadPackt, usDataLen))
	{
		ClearPercentage();
		SetRedaingState(eReadingFromFlash);
		memcpy(&usDownldDta, ucDownloadPackt, sizeof(usDownldDta));

		if(0 != (memcmp(sConfigurationData.ucSubjectUhiId, usDownldDta.ucSubjectUhiId,SUBJECT_UHI_ID_LEN)))
		{
			SetDaqErrorStatus(eErrorUhiIdMisMatch);
		}

		ClearDataSet();
		vlRetVal = true;
	}

	return vlRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : //
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
void ConfigDataSave()
{
	HAL_Delay(20);
	FlashSaveConfigurationData((uint8_t *)&sConfigurationData);
	FlashErasePatientResultData();
	configflag = false;
}
/*-----------------------------------------------------------------------------
//Purpose   : //
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
void SetConfigFlag()
{
	configflag = true;
}
/*-----------------------------------------------------------------------------
//Purpose   : //
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
bool GetConfigFlag()
{
	return configflag;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Set App init message Details Message
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool HandleAppInitMessagePckt(uint8_t* ucAppnitPacket, uint16_t usDataLen)
{
	bool blRetVal 					= false;
	sAppInitRequest sAppInitPckt 	= {0};

	if(ucAppnitPacket)
	{
		if (checkCRC(ucAppnitPacket, usDataLen))
		{
			memcpy(&sAppInitPckt.ucReq_type, ucAppnitPacket, usDataLen);

			if (sAppInitPckt.ucInitCmd == 'I')
			{
				/* Process the time stamp here */
				SetRtcTime((char*)&sAppInitPckt.ucTimeStamp[0]);
				blRetVal = true;
			}
		}
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Set App init message response builder
//Inputs    : None
//Outputs   : None
//Return    :
//Notes     : None
-----------------------------------------------------------------------------*/
static bool BuildAppInitMessageResponse()
{
	bool blRetVal 						= false;
	sAppInitRespMsg sAppInitRespPckt 	= {0};
	_sDeviceInfoData sDeviceInfo		= {0};

	sAppInitRespPckt.ucReq_type		= eAppInitResp + '0';
	sAppInitRespPckt.ucPacketLen	= sizeof(sAppInitRespPckt);
	sAppInitRespPckt.ucErrorCode	= GetDaqErrorStatus();


	GetDeviceInfo(&sDeviceInfo);
	memcpy(sAppInitRespPckt.ucDaqId,sDeviceInfo.ucDeviceId, DAQ_ID_LEN);
	memcpy(sAppInitRespPckt.ucSubjectWdId,sDeviceInfo.ucWearableId, SUBJECT_WD_ID_LEN);
	memcpy(sAppInitRespPckt.ucSubjectWdSize,ucpWearableSize[sDeviceInfo.eWearableSize-1],strlen(ucpWearableSize[sDeviceInfo.eWearableSize -1 ]));  //strlen(ucpWearableSize[sDeviceInfo.eWearableSize -1 ])
	sAppInitRespPckt.ucBatteryPercentage 	= GetBatteryPercentage();

	sAppInitRespPckt.unCheckSum=calculateCRC((unsigned char*) &sAppInitRespPckt, sizeof(sAppInitRespPckt) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sAppInitRespPckt, sizeof(sAppInitRespPckt));
		blRetVal = true;
	}

	return blRetVal;
}

extern void handle_flash_write();

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Set config Details Message
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool HandleSetConfigDetailsPckt(uint8_t* ucConfigPacket, uint16_t usDataLen)
{
	bool vlRetVal 					= false;
	RTC_TimeTypeDef t 				= {0};
	_sSetConfig sTempData 			= {0};
	_sDeviceInfoData sGetdevInfo 	= {0};

	if(ucConfigPacket )
	{
		if (checkCRC(ucConfigPacket, usDataLen))
		{
			memcpy(&sTempData.ucReq_type, ucConfigPacket, usDataLen);
			memcpy(&sConfigurationData, &(sTempData.sSetConfData), sizeof(sConfigurationData));
			SetRtcTime((char*)&sConfigurationData.ucTimeStamp[0]);
			GetDeviceInfo(&sGetdevInfo);

			if(0 != (memcmp(sGetdevInfo.ucDeviceId, sConfigurationData.ucDaqId,DEVICEID_LEN)))
			{
				SetDaqErrorStatus(eErrorDaqIdMisMatch);
			}
			if(0 != (memcmp(sGetdevInfo.ucWearableId, sConfigurationData.ucSubjectWdId,DAQID_LEN)))
			{
				SetDaqErrorStatus(eErrorWearableIdMisMatch);
			}
			if(0 != (memcmp(ucpWearableSize[sGetdevInfo.eWearableSize-1], sConfigurationData.ucSubjectWdSize,strlen(sConfigurationData.ucSubjectWdSize))))
			{
				SetDaqErrorStatus(eErrorWearableSizeMisMatch);
			}
#if 1
			FlashSaveConfigurationData((uint8_t *)&sConfigurationData);
			handle_flash_write();
//			FlashErasePatientResultData();
#endif
			vlRetVal = true;
			GetRtcTime(&t);
			BasicInfoRegistered();
			SetConfigFlag();
		}
	}
	return vlRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Get config
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool HandleGetSavedConfigurationDetails(uint8_t* ucConfigGetPacket, uint16_t usDataLen)
{
	bool vlRetVal 				= false;
	_sGetConfig spGetConfig 	= { 0 };

	if(ucConfigGetPacket && usDataLen )
	{
		if (checkCRC(ucConfigGetPacket, usDataLen))
		{
			memcpy(&spGetConfig, ucConfigGetPacket, sizeof(spGetConfig));

			if(0 != (memcmp(sConfigurationData.ucSubjectUhiId, spGetConfig.ucSubjectUhiId,SUBJECT_UHI_ID_LEN)))
			{
				SetDaqErrorStatus(eErrorUhiIdMisMatch);
			}

			vlRetVal = true;
		}
	}

	return vlRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Parsing Get config
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool HandleBuzzerPlayRequest(uint8_t* ucConfigPacket, uint16_t usDataLen)
{
	bool vlRetVal 					= false;
	_sBuzzerReqCmnd spBuzzerReq 	= { 0 };

	if(ucConfigPacket && usDataLen )
	{
		if (checkCRC(ucConfigPacket, usDataLen))
		{
			memcpy(&spBuzzerReq, ucConfigPacket, sizeof(ucConfigPacket));

			BuzzerPlayPiezo();
			//BuzzerPlay(10);


			vlRetVal = true;
		}
	}

	return vlRetVal;

}
/*-----------------------------------------------------------------------------
//Purpose   : Parsing Get config
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool BuildGetConfigDetailsResponse()
{
	bool blRetVal						= false;
	_sGetConfigResp sGetConfigResponse	= {0};

	sGetConfigResponse.ucReq_type			= eGetConfigDetailsResp + '0';
	sGetConfigResponse.ucPacketLen			= sizeof(sGetConfigResponse);
	sGetConfigResponse.sErrorCode			= GetDaqErrorStatus();
	sGetConfigResponse.ucStatus				= 'S';
	sGetConfigResponse.ucBatteryPercentage 	= GetBatteryPercentage();

	FlashReadConfigurationData((void*) &sConfigurationData);
	memcpy(&sGetConfigResponse.sSavedConfData,&sConfigurationData, sizeof(sGetConfigResponse.sSavedConfData));

#if 0
	// Send already saved time stamp for now
 	if (GetRtcTime(&sCurrentTime) && GetRtcDate(&sCurrentDate))
 	{
 		memset(sGetConfigResponse.sSavedConfData.ucTimeStamp, 0X00, TIME_STAMP_LEN);

 		sprintf((char*)sGetConfigResponse.sSavedConfData.ucTimeStamp,"%02d%02d%02d%02d%02d%02d",
		 			sCurrentDate.Date, sCurrentDate.Month, sCurrentDate.Year,
							sCurrentTime.Hours, sCurrentTime.Minutes, sCurrentTime.Seconds);
 	}
#endif

 	sGetConfigResponse.ucCheckSum = calculateCRC((unsigned char*)&sGetConfigResponse,sizeof(sGetConfigResponse) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sGetConfigResponse, sizeof(sGetConfigResponse));
		blRetVal = true;
		BasicInfoDownloaded();
	}

	return blRetVal;

}

/*-----------------------------------------------------------------------------
//Purpose   : Save config Data to flash
//Inputs    : ucDownloadPackt - download Packet, usDataLen - length of the packet
//Outputs   : None
//Return    : true - Reply Needed, false - reply not needed
//Notes     : None
-----------------------------------------------------------------------------*/
static bool BuildSetConfigurationResponse()
{
	_sSetConfigResp sSetConfigResponce 	= {0};
	bool blRetVal 						= false;

	/* Check status here and generate error code */
	sSetConfigResponce.ucReq_type			= eSetConfigResp + '0';
	sSetConfigResponce.ucPacketLen			= sizeof(_sSetConfigResp);
	sSetConfigResponce.sErrorCode			= GetDaqErrorStatus();

	sSetConfigResponce.ucStatus				= 'S';
	sSetConfigResponce.ucBatteryPercentage 	= GetBatteryPercentage();

	sSetConfigResponce.ucCheckSum = calculateCRC((unsigned char*)&sSetConfigResponce,sizeof(_sSetConfigResp) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sSetConfigResponce, sizeof(_sSetConfigResp));
		blRetVal = true;
	}

	return blRetVal;
}


/*-----------------------------------------------------------------------------
//Purpose   : clear the error status of the device
//Inputs    : eErrorStatus - status
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void ClearDaqErrorStatus(_eErrorCode eErrorStatus)
{
	eDaqErrorStatus &= ~( 0x01 << eErrorStatus);
}


/*-----------------------------------------------------------------------------
//Purpose   : Set the error status of the device
//Inputs    : eErrorStatus - status
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void SetDaqErrorStatus(_eErrorCode eErrorStatus)
{
	eDaqErrorStatus |= eErrorStatus;
}

/*-----------------------------------------------------------------------------
//Purpose   : Set the error status of the device
//Inputs    : None
//Outputs   : None
//Return    : eErrorStatus - status
//Notes     : None
-----------------------------------------------------------------------------*/
_eErrorCode GetDaqErrorStatus( void)
{

	return SWAPBYTES(eDaqErrorStatus);
}

/*-----------------------------------------------------------------------------
//Purpose   : Set the error status of the device
//Inputs    : None
//Outputs   : None
//Return    : eErrorStatus - status
//Notes     : None
-----------------------------------------------------------------------------*/
static void ClearAllErrorStatus()
{
	eDaqErrorStatus = eErrorNone;
}

/*-----------------------------------------------------------------------------
//Purpose   : Set the error status of the device
//Inputs    : None
//Outputs   : None
//Return    : eErrorStatus - status
//Notes     : None
-----------------------------------------------------------------------------*/
static bool BuildBuzzerResponce()
{
	_sBuzzerResp sSetBuzzerPlayReq 		= {0};
	bool blRetVal						= false;
	_sDeviceInfoData sDeviceInfo		= {0};

	GetDeviceInfo(&sDeviceInfo);

	/* Check status here and generate error code */
	sSetBuzzerPlayReq.ucReq_type	= eBuzzerResponce + '0';
	sSetBuzzerPlayReq.ucPacketLen	= sizeof(_sBuzzerResp);
	sSetBuzzerPlayReq.sErrorCode	= GetDaqErrorStatus();

	memcpy(sSetBuzzerPlayReq.ucDaqId, sDeviceInfo.ucDeviceId, DAQ_ID_LEN);
	sSetBuzzerPlayReq.ucBatteryPercentage = GetBatteryPercentage();
	sSetBuzzerPlayReq.ucCheckSum = calculateCRC((unsigned char*)&sSetBuzzerPlayReq,sizeof(sSetBuzzerPlayReq) - 1);

	if (!BleTxDataBaseStatus())
	{
		SendMessageToBle(&sSetBuzzerPlayReq, sizeof(sSetBuzzerPlayReq));
		blRetVal = true;
	}

	return blRetVal;

}
