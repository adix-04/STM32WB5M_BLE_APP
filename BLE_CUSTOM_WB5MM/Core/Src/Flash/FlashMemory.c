/*
 * FlashMemory.c
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */

#include "FlashMemory.h"
#include "utilities_conf.h"
#include "PacketHandler.h"
#include "RTCC.h"
#include "main.h"

#define FLASH_READ_WRITE_MAX_COUNT	48
#define MAX_ITERATION				10

extern long double a[MAX_PROBE_COUNT];
extern long double b[MAX_PROBE_COUNT];
extern long double c[MAX_PROBE_COUNT];
extern long double d[MAX_PROBE_COUNT];

/*-----------------------------------------------------------------------------
//Purpose   : Get flash page number
//Inputs    : Addr - Address in the falsh
//Outputs   : None
//Return    : Page number
//Notes     : None
-----------------------------------------------------------------------------*/
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}

/*-----------------------------------------------------------------------------
//Purpose   : Save the data to flash
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void FlashWriteWearableConstants(void)
{
	bool bRetVal = false;
	uint32_t FirstPage = 0, NbOfPages = 0;
	uint32_t Address = 0, PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	int i = 0;
    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    FirstPage = GetPage(ADDR_FLASH_PAGE_01);

    NbOfPages = 2;				//GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	bRetVal = false;
    }
    else
    {
    	i=0;
    	Address = FLASH_ADDR_A_START;

    	while(i<48)
    	{
    		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (int64_t)a[i]) == HAL_OK)
     		{
    	    	Address = Address + 8;  /* increment to next double word*/
    			i++;
    		}
    		else
    		{
    			bRetVal = false;
    			break;
    		}
    	}

    	i=0;
    	Address = FLASH_ADDR_B_START;

    	while(i<48)
    	{
    		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (int64_t)b[i]) == HAL_OK)
    	    {
    	       	Address = Address + 8;  /* increment to next double word*/
    	    	i++;
    	    }
    	    else
    	    {
    	    	bRetVal = false;
    	    	break;
    	    }
   	    }

    	i = 0;
    	Address = FLASH_ADDR_C_START;

    	while(i < 48)
    	{
    		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (int64_t)c[i]) == HAL_OK)
     		{
    	    	Address = Address + 8;  /* increment to next double word*/
    			i++;
    		}
    		else
    		{
    			bRetVal = false;
    			break;
    		}
    	}

    	i=0;
    	Address = FLASH_ADDR_D_START;

    	while(i < 48)
    	{
    		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (int64_t)d[i]) == HAL_OK)
    	    {
    	       	Address = Address + 8;  /* increment to next double word*/
    	    	i++;
    	    }
    	    else
    	    {
    	    	bRetVal = false;
    	    	break;
    	    }
   	    }

    }
    HAL_FLASH_Lock();

    (void)(bRetVal);
}

/*-----------------------------------------------------------------------------
//Purpose   : Read wearable contsants from flash
//Inputs    : Addr - Address in the falsh, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void FlashReadWearableConstants(uint32_t Address, long double * dataPtr )
{
	int64_t data32 		= 0;
	int64_t data32_1 	= 0;
	unsigned int i		= 0;

	while(i < 48)
	{
    	data32 		= *(__IO uint32_t *)Address;
    	data32_1 	= *(__IO uint32_t *)(Address + 4U);
    	data32 		= (int64_t)((data32_1 << 32U)| data32);
    	*(dataPtr+i)= (long double)(*(&data32) * 0.000000000000000001);
    	Address = Address + 8;
    	i++;
    }
}

/*-----------------------------------------------------------------------------
//Purpose   : REtreive configuration values from falsh memory
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void getSavedConfigurationValues(void)
{
	FlashReadWearableConstants(FLASH_ADDR_A_START, a);
	FlashReadWearableConstants(FLASH_ADDR_B_START, b);
	FlashReadWearableConstants(FLASH_ADDR_C_START, c);
	FlashReadWearableConstants(FLASH_ADDR_D_START, d);
}

/*-----------------------------------------------------------------------------
//Purpose   : Read wearable patient result data from flash
//Inputs    : Addr - Address in the falsh, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
extern uint8_t ucTestBuff[8];

timeStampType timeStampSave;


uint32_t FlashSavePatientResultData (float *fData, uint8_t resetReadAddress)
{

	FLASH_EraseInitTypeDef EraseInitStruct;
 	uint32_t PAGEError;
 	static uint32_t StartPageAddress = ADDR_FLASH_PAGE_02;
 	static uint8_t 	flashWriteCount = 0;
	/*
	 * this variable is to increment each time this function is called.
	 * so rename it accordingly. Bcuz this variable is used to reset StartPageAddress
	 */
 	uint64_t dataToWrite;
 	int probeNo = 0;
 	static uint8_t ucReadItr = 0;
	float Data[48] = {0};
	RTC_TimeTypeDef sCurrentTime  = {0};
	uint8_t ucTempBuff[8] = {0};// no longer needed....
	RTC_DateTypeDef sCurrentDate = {0};
	static int eRROR = 0;

 	memcpy(Data,fData,sizeof(Data));

 	if (ucReadItr == 10)
 	{
 		ucReadItr = 0;
 		return 0;
 	}
#if 0
	if(resetReadAddress == 1)
		StartPageAddress = ADDR_FLASH_PAGE_02;
 	/* Unlock the Flash to enable the flash control register access *************/
 	HAL_FLASH_Unlock();

 	/* Erase the user Flash area*/
 	uint32_t StartPage = GetPage(ADDR_FLASH_PAGE_02);
 	uint32_t EndPageAdress = StartPageAddress + MAX_PROBE_COUNT*4;
 	uint32_t EndPage = GetPage(EndPageAdress);














 	if(resetReadAddress)
 	{
 	/* Fill EraseInit structure*/
		EraseInitStruct.Page		= Start		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
Page;
		EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			/*Error occurred while page erase.*/
			return HAL_FLASH_GetError ();
		}
 	}
#endif

#if 1
 	if(flashWriteCount == 0)
 			StartPageAddress = ADDR_FLASH_PAGE_02;
 	 	/* Unlock the Flash to enable the flash control register access *************/
 	 	HAL_FLASH_Unlock();

 	 	/* Erase the user Flash area*/
 	 	uint32_t StartPage = GetPage(ADDR_FLASH_PAGE_02);
 	 	uint32_t EndPageAdress = StartPageAddress + MAX_PROBE_COUNT*4;
 	 	uint32_t EndPage = GetPage(EndPageAdress);

 	 	if(flashWriteCount == 0)
 	 	{
 	 	/* Fill EraseInit structure*/
 			EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
 			EraseInitStruct.Page		= StartPage;
 			EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

 			if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
 			{
 				/*Error occurred while page erase.*/
 				return HAL_FLASH_GetError ();
 			}
 	 	}
#endif
 	/* Program the user Flash area word by word*/
 	while (probeNo < MAX_PROBE_COUNT)
	{
 		Data[probeNo] *= 10000;
 		Data[probeNo+1] *= 10000;
 		dataToWrite = Data[probeNo+1];
 		dataToWrite = (dataToWrite<<32);
 		dataToWrite |= (uint32_t)Data[probeNo];

 		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, (uint64_t) dataToWrite) == HAL_OK)
 		{
 			StartPageAddress += 8;  // use StartPageAddress += 2 for half word and 8 for double word
 	    	probeNo += 2;
 		}
 	    else
 	    {
 	    	/* Error occurred while writing data in Flash memory*/
 	    	return HAL_FLASH_GetError ();
 	    }
	}

 	ucReadItr ++;

 	if (GetRtcTime(&sCurrentTime) && GetRtcDate(&sCurrentDate))
 	{
// 		memcpy(ucTempBuff, &sCurrentTime, sizeof(RTC_TimeTypeDef)); //comment arun
 		timeStampSave.ucTestBuff[0] = sCurrentDate.Date;
 		timeStampSave.ucTestBuff[1] = sCurrentDate.Month;
 		timeStampSave.ucTestBuff[2] = sCurrentDate.Year;
 		timeStampSave.ucTestBuff[3] = sCurrentTime.Hours;
 		timeStampSave.ucTestBuff[4] = sCurrentTime.Minutes;
 		timeStampSave.ucTestBuff[5] = sCurrentTime.Seconds;

// 		memcpy(ucTempBuff,ucTestBuff,8);


 		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, (uint64_t)timeStampSave.ucTestData) != HAL_OK)
 		{
 			eRROR++;

 			// update error
		}


 		StartPageAddress += (sizeof(RTC_TimeTypeDef) + TIMESTAMP_MEMORY_ROUND_CONST);
 		flashWriteCount++;
 		if(flashWriteCount >= MAX_ITERATION)
 			flashWriteCount = 0;
 	}

 	/* Lock the Flash to disable the flash control register access (recommended
 	to protect the FLASH memory against possible unwanted operation)
 	*********/
 	HAL_FLASH_Lock();

 	return 0;
}

/*-----------------------------------------------------------------------------
//Purpose   : Read wearable patient result data from flash
//Inputs    : Addr - Address in the falsh, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
void FlashReadPatientResultData(float *Data, uint8_t resetReadAddress,uint8_t *sReadTime)
{
	uint32_t readData 					= 0;
	static uint32_t StartPageAddress 	= ADDR_FLASH_PAGE_02;
	unsigned char probeNo 				= 0;
	static uint8_t ucReadCount 			= 0;
	static uint8_t flashReadCount = 0;
	/*
	 * this variable is to increment each time this function is call ed.
	 * so rename it accordingly. Bcuz this variable is used to reset StartPageAddress
	 */

#if 0
	if(resetReadAddress == 0)
	{
		StartPageAddress = ADDR_FLASH_PAGE_02;
	}
#endif
#if 1
	if(flashReadCount == 0)
		StartPageAddress = ADDR_FLASH_PAGE_02;
#endif
	while (probeNo < MAX_PROBE_COUNT)
	{
		readData = *(__IO uint32_t *)StartPageAddress;
		StartPageAddress += 4;
		*Data = (float)(*(&readData) * 0.0001);
		Data++;
		probeNo++;
		ucReadCount++;
	}

	memcpy(sReadTime, (uint32_t *)StartPageAddress,8);
//	sReadTime+=4;
	StartPageAddress += 24;

	flashReadCount++;
	if(flashReadCount >= MAX_ITERATION)
	{
		flashReadCount = 0;
		StartPageAddress 	= ADDR_FLASH_PAGE_02;
	}
}

/*-----------------------------------------------------------------------------
//Purpose   : write wearable patient result data to flash
//Inputs    : Addr - Address in the falsh, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint32_t FlashSaveConfigurationData (uint8_t *ucData)
{

	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	uint32_t StartPageAddress = ADDR_FLASH_PAGE_03;
	uint64_t dataToWrite;
	uint8_t ucTotalIteration = (sizeof(_sSetConfigFlashData) / 8) + 1;
	uint8_t ucCount = 0;
	uint8_t ucTempVer[10] = {0};
	_uFlahWrite64Bit uFlashData = {0};

	if (NULL == ucData)
	{
		return 0;
	}
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/
	uint32_t StartPage = GetPage(ADDR_FLASH_PAGE_03);
	uint32_t EndPageAdress = StartPageAddress;
	uint32_t EndPage = GetPage(EndPageAdress);

	if(1)
	{
	/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase	= FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Page		= StartPage;
		EraseInitStruct.NbPages		= ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			/*Error occurred while page erase.*/
			return HAL_FLASH_GetError ();
		}
 	}

 	/* Program the user Flash area word by word*/
 	while (ucTotalIteration--)
	{
 		//dataToWrite = (uint64_t)ucData[ucCount * 8 ];
 		 memcpy(uFlashData.ucTestBuff, &ucData[ucCount * 8 ], 8);

 		if (0 == ucTotalIteration)
 		{
 			memcpy(uFlashData.ucTestBuff,&ucData[ucCount * 8],(sizeof(_sSetConfigFlashData) - ((sizeof(_sSetConfigFlashData) / 8) * 8)));
 			dataToWrite = (uint64_t)ucTempVer[0];
 		}

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, (uint64_t) uFlashData.ucTestData) == HAL_OK)
		{
			StartPageAddress += 8;  // use StartPageAddress += 2 for half word and 8 for double word
			ucCount++;
			memset(uFlashData.ucTestBuff,0x00,8);
		}
		else
		{
			/* Error occurred while writing data in Flash memory*/
			return HAL_FLASH_GetError ();
		}
//		HAL_Delay(1);
	}

	/* Lock the Flash to disable the flash control register access (recommended
	to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return 0;
}

/*-----------------------------------------------------------------------------
//Purpose   : Read wearable patient configuration data from flash
//Inputs    : Addr - Address in the flash, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
bool FlashReadConfigurationData(void *ucData)
{
	bool blRetVal = false;
	uint32_t StartPageAddress = ADDR_FLASH_PAGE_03;

	if (ucData)
	{
		memcpy (ucData, ( uint32_t *)StartPageAddress, sizeof(_sSetConfigFlashData));
		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : write wearable patient result data to flash
//Inputs    : Addr - Address in the falsh, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint32_t FlashSaveDeviceInfo (uint8_t *ucData)
{

	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	uint32_t StartPageAddress = (uint32_t)ADDR_FLASH_PAGE_04;
	uint64_t dataToWrite;
	uint8_t ucTotalIteration = (sizeof(_sDeviceInfoData) / 8) + 1;
	uint8_t ucCount = 0;
	uint8_t ucTempVer[8] = {0};
	_uFlahWrite64Bit uFlashData = {0};

	if (NULL == ucData)
	{
		return 0;
	}

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/
	uint32_t StartPage		= GetPage(ADDR_FLASH_PAGE_04);
	uint32_t EndPageAdress	= StartPageAddress;
	uint32_t EndPage		= GetPage(EndPageAdress);

	if(1)
	{
		/* Fill EraseInit structure */
		EraseInitStruct.TypeErase	= FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Page		= StartPage;
		EraseInitStruct.NbPages		= ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			/*Error occurred while page erase.*/
			return HAL_FLASH_GetError ();
		}
	}

	/* Program the user Flash area word by word*/
	while (ucTotalIteration--)
	{
		//dataToWrite = (uint64_t)ucData[ucCount * 8 ];

#if 0
		if (0 == ucTotalIteration)
		{
			memcpy(uFlashData.ucTestBuff,ucData,(sizeof(_sDeviceInfoData) - ((sizeof(_sDeviceInfoData) / 8) * 8)));
		}
		else
		{
#endif
			memcpy(uFlashData.ucTestBuff, &ucData[ucCount * 8 ], 8);
#if 0
		}
#endif

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, (uint64_t) uFlashData.ucTestData) == HAL_OK)
		{
			StartPageAddress += 8;  // use StartPageAddress += 2 for half word and 8 for double word
			ucCount++;
			memset(uFlashData.ucTestBuff, 0x00, 8);
		}
		else
		{
			/* Error occurred while writing data in Flash memory*/
			return HAL_FLASH_GetError ();
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return 0;
}

/*-----------------------------------------------------------------------------
//Purpose   : Read wearable patient configuration data from flash
//Inputs    : Addr - Address in the flash, dataPtr - pointer to the data
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
bool FlashReadDeviceId(_sDeviceInfoData *ucData)
{
    bool blRetVal 				= false;
	uint32_t StartPageAddress	= ADDR_FLASH_PAGE_04;

	if (ucData)
	{
		memcpy (ucData, ( uint32_t *)StartPageAddress, (sizeof(_sDeviceInfoData)));

		if ((ucData->ucDeviceId[0] != 'M') || (ucData->ucDeviceId[1] != 'D'))
		{
			memcpy(ucData->ucDeviceId, DAQ_ID, LENGTH_OF_DEV_ID);
		}
		if ((ucData->ucWearableId[0] != 'W') || (ucData->ucWearableId[1] != 'D'))
		{
			memcpy(ucData->ucWearableId, WEARABLE_ID, LENGTH_OF_WEARABLE_ID);
		}
		if(((ucData->eWearableSize - 1) > eWearableFullSize) || ((ucData->eWearableSize - 1) < eWearableSizeInvalid))
		{
			ucData->eWearableSize = eWearableFullSize;
		}



		blRetVal = true;
	}

	return blRetVal;
}

/*-----------------------------------------------------------------------------
//Purpose   : Erase patient result data after receving the basic info
//Inputs    : None
//Outputs   : None
//Return    : None
//Notes     : None
-----------------------------------------------------------------------------*/
uint32_t FlashErasePatientResultData(void)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	uint32_t StartPageAddress = ADDR_FLASH_PAGE_02;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/
	uint32_t StartPage = GetPage(ADDR_FLASH_PAGE_02);
	uint32_t EndPageAdress = StartPageAddress;
	uint32_t EndPage = GetPage(EndPageAdress);


	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase	= FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page		= StartPage;
	EraseInitStruct.NbPages		= ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/*Error occurred while page erase.*/
		return HAL_FLASH_GetError ();
	}
	else
	{
		return 0;
	}
}
extern float FINAL_TEMP[48];
/*-----------------------------------------------------------------------------
//Purpose   : Check the validity of last write
//Inputs    : None
//Outputs   : None
//Return    : true : valid data, false : no data / data corrupted
//Notes     : None
-----------------------------------------------------------------------------*/
bool CheckValidityOfLastRead()
{
	bool ret = false;

	uint32_t uStartAddress = ADDR_FLASH_PAGE_02 + (sizeof(FINAL_TEMP) * 9 )+ (sizeof(uint64_t) * 9);

	//add some more checking if needed
	if (0xFFFFFFFF != *(uint32_t*)uStartAddress)
	{
		ret = true;
	}

	return ret;
}
