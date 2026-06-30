/*
 * FlashMemory.h
 *
 *  Created on: feb 27, 2023
 *      Author: Alex
 */

#ifndef FLASH_MEMORY
#define FLASH_MEMORY

#include "stm32wbxx_hal.h"
#include "stdbool.h"

#define MAX_PROBE_COUNT     			48
#define TIMESTAMP_MEMORY_ROUND_CONST	4	// 8 changed to 4

#define ADDR_FLASH_PAGE_01  	((uint32_t)0x08030000) /* points to page 126 in REF manuael Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802
#define ADDR_FLASH_PAGE_02  	((uint32_t)0x08031000) /* points to page 126 in REF manuael Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802
#define ADDR_FLASH_PAGE_03  	((uint32_t)0x08032000)
#define ADDR_FLASH_PAGE_04  	((uint32_t)0x08033000)

#define FLASH_ADDR_A_START		((uint32_t)0x08030000) /* Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802
#define FLASH_ADDR_B_START		((uint32_t)0x08030180) /* Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802
#define FLASH_ADDR_C_START		((uint32_t)0x08030300) /* Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802
#define FLASH_ADDR_D_START		((uint32_t)0x08030480) /* Base @ of Page 16, 4 Kbytes */ //modified... changed address from 0x801 to 0x802



typedef union {
	uint8_t ucTestBuff[8];
	uint64_t ucTestData;
}timeStampType;

typedef union {
	uint8_t 	ucTestBuff[8];
	uint64_t 	ucTestData;
}_uFlahWrite64Bit;

void getSavedConfigurationValues(void);
void FlashReadWearableConstants(uint32_t Address, long double * dataPtr );
void FlashWriteWearableConstants(void);
uint32_t FlashSavePatientResultData (float *Data, uint8_t resetReadAddress);
void FlashReadPatientResultData(float *Data, uint8_t resetReadAddress, uint8_t *sReadTime);
uint32_t FlashSaveConfigurationData (uint8_t *ucData);
uint32_t FlashErasePatientResultData(void);
bool FlashReadConfigurationData(void *ucData);
bool CheckValidityOfLastRead();

#endif
