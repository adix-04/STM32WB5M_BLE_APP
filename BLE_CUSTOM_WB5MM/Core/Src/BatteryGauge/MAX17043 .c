/******************************************************************************
file name 	: - MAX17043.c
Auther 		: - Alexd

This file implements all functions of the MAX17043. 

******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "MAX17043.h"
#include "stm32wbxx_hal.h"

/* MAX17043 Register Definitions, all registers contain two bytes of data and span two addresses. */
#define MAX17043_VCELL					0x02	// R - 12-bit A/D measurement of battery voltage
#define MAX17043_SOC					0x04	// R - 16-bit state of charge (SOC)
#define MAX17043_MODE					0x06	// W - Sends special commands to IC
#define MAX17043_VERSION				0x08	// R - Returns IC version
#define MAX17043_CONFIG					0x0C	// R/W - Battery compensation (default 0x971C)
#define MAX17043_COMMAND				0xFE	// W - Sends special comands to IC
#define MAX17043_CONFIG_SLEEP			7		// MAX17043 Config Register Bits 
#define MAX17043_CONFIG_ALERT			5
#define MAX17043_CONFIG_THRESHOLD		0
#define MAX17043_MODE_QUICKSTART		0x4000	// MAX17043 Mode Register Commands
#define MAX17043_COMMAND_POR			0x5400	// MAX17043 Command Register Commands
#define MAX17043_ADDRESS				0x36	// MAX17043 7-Bit I2C Address 
#define BAT_GUAGE_DELAY					250
#define CONSTRAIN(amt, low, high)		((amt) < (low) ? (low) : ((amt)  >(high) ? (high) : (amt)))

/* Function declarations */
static uint8_t WriteGauge(uint16_t data, uint8_t address);
static uint16_t ReadFromGauge(uint8_t address);

/*Global variables */
extern I2C_HandleTypeDef hi2c3;

/* Function defenition */

/****************************************************************************************
// Function      : MAX17043 init
// Description   : Initilize the max battery gauge
// Parameters    : None
// Return        : None.
****************************************************************************************/
void MAX17043_Init()
{
    uint8_t data[2];

    // Set MAX17043 configuration
    data[0] = 0x05;  // Configuration register address
    data[1] = 0x9710; // Example configuration value
    HAL_I2C_Master_Transmit(&hi2c3, MAX17043_ADDRESS, data, 2, MAX17043_MODE);
}

/****************************************************************************************
// Function      : Quick Start
// Description   : A quick-start allows the MAX17043 to restart fuel-gauge calculations in the
                 : same manner as initial power-up of the IC. If an application’s power-up
                 : sequence is exceedingly noisy such that excess error is introduced into the
                 : IC’s “first guess” of SOC, the host can issue a quick-start to reduce the
                 : error. A quick-start is initiated by a rising edge on the QSTRT pin, or
                 : through software by writing 4000h to MODE register.
// Parameters    : none
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t QuickStartBatteryGauge()
{
	return WriteGauge(MAX17043_MODE_QUICKSTART, MAX17043_MODE);
}

/****************************************************************************************
// Function      : Get the MAX17043's voltage reading.
// Description   : This function will read the voltage
// Parameters    : none
// Return        : floating point value between 0-5V in 1.25mV increments.
****************************************************************************************/
void GetBatteryVoltage(float *vBatV)
{
	uint16_t vCell;

	vCell = ReadFromGauge(MAX17043_VCELL);
	/* vCell is a 12-bit register where each bit represents 1.25mV */
	vCell = (vCell) >> 4;

	*vBatV = (float) (vCell / 800.0);
}

/****************************************************************************************
// Function      : Get remaining battery percentage 
// Description   : Get the MAX17043's state-of-charge (SOC) reading, as calculated
                 : by the IC's "ModelGauge" algorithm.
// Parameters    : none
// Return        : floating point value between 0-100, representing a percentage of
                 : full charge.
****************************************************************************************/

#define toHighByte(w)             ((uint8_t) ((w) >> 8))
#define toLowByte(w)              ((uint8_t) ((w) & 0xff))

float GetBatteryChargePercentage(float *Temp)
{
	uint16_t soc  = 0;
	float Perc = 0;


	soc = ReadFromGauge(MAX17043_SOC);

	  float percentage = (float)toHighByte(soc);

	  //
	  // The low byte contains additional resolution of 1/256%.
	  //
	  float fraction = (float)(toLowByte(soc) / 256.f);

	  Perc =  (percentage + fraction);
	  *Temp = Perc;

	  return Perc;
}

/****************************************************************************************
// Function      : Get battery gauge version
// Description   : Get the MAX17043's production version number.
// Parameters    : none
// Return        : 3 on success
****************************************************************************************/
uint16_t GetBatteryGaugeVersion()
{
	return ReadFromGauge(MAX17043_VERSION);
}

/****************************************************************************************
// Function      : Get low battery threshold voltage
// Description   : Get the MAX17043's current percentage threshold that will
                 : trigger an alert.
// Parameters    : none
// Return        : An integer value between 1 and 32, representing a % that will
                 : trigger an alert interrupt.
****************************************************************************************/
uint8_t GetBatteryVoltageThreshold()
{
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);
	uint8_t threshold = (configReg & 0x001F);

	/* It has an LSb weight of 1%, and can be programmed from 1% to 32%.
		 Values are in 2's complement, e.g.: 00000=32%, 00001=31%, 11111=1%.
		 Let's convert our percent to that first:
	*/
	threshold = 32 - threshold;
	return threshold;
}

/****************************************************************************************
// Function      : Set Low battery threshold voltage
// Description   : Set the MAX17043's percentage threshold that will trigger an alert.
// Parameters    : Percentage value that will trigger an alert interrupt. Any value between 1 and 32 is valid.
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t SetBatteryVoltageThreshold(uint8_t percent)
{
	/*
		The alert threshold is a 5-bit value that sets the state of charge level
		where an interrupt is generated on the ALRT pin.	
		It has an LSb weight of 1%, and can be programmed from 1% to 32%.
		Values are in 2's complement, e.g.: 00000=32%, 00001=31%, 11111=1%.
		Let's convert our percent to that first:
	*/
	percent = CONSTRAIN(percent, 0, 32);
	percent = 32 - percent;

	/* Read config reg, so we don't modify any other values: */
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);
	/* Mask out threshold bits */
	configReg &= 0xFFE0;
	/* Add new threshold */
	configReg |= percent;

	return WriteGauge(configReg, MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : Clear low battery Alert
// Description   : Clear the MAX17043's alert flag.
// Parameters    : none
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t ClearBatteryLowAlert()
{
	/* Read config reg, so we don't modify any other values: */
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);
	/* Clear ALRT bit manually. */
	configReg &= ~(1 << MAX17043_CONFIG_ALERT); 

	return WriteGauge(configReg, MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : Get low battery alert
// Description   : Check if the MAX17043's alert interrupt has been set.
// Parameters    : [clear] - If [clear] is true, the alert flag will be cleared if it
// Return        : Returns 1 if interrupt is/was triggered, 0 if not.
****************************************************************************************/
uint8_t GetBatteryLowAlert(bool clear)
{
	/* Read config reg, so we don't modify any other values: */
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);

	if (configReg & (1 << MAX17043_CONFIG_ALERT))
	{
		if (clear)
		{
			/* Clear ALRT bit manually. */
			configReg &= ~(1 << MAX17043_CONFIG_ALERT);
			WriteGauge(configReg, MAX17043_CONFIG);
		}
		return 1;
	}

	return 0;
}

/****************************************************************************************
// Function      : Set sleep mode
// Description   : Set the MAX17043 into sleep mode.
// Parameters    : none
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t SleepBatteryGauge()
{
	/* Read config reg, so we don't modify any other values: */
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);

	if (configReg & (1 << 7))
	/* Already sleeping, do nothing but return an error */
		return 5; 
	configReg |= (1 << 7);
	/* Set sleep bit */

	return WriteGauge(configReg, MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : wake abttery gauge
// Description   : Wake the MAX17043 up from sleep.
// Parameters    : none
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t BattteryGaugeWake()
{
	/* Read config reg, so we don't modify any other values:*/
	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);

	if (!(configReg & (1<<7)))
	 /* Already awake, do nothing but return an error */
		return 5;
	configReg &= ~(1<<7); 
	/* Clear sleep bit */

	return WriteGauge(configReg, MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : To reset the battery gauge
// Description   : Issue a Power-on-reset command to the MAX17043. This function
                 : will reset every register in the MAX17043 to its default value.
// Parameters    : none
// Return        :  0 on success, positive integer on fail.
****************************************************************************************/
uint8_t BattteryGaugeReset()
{
	return WriteGauge(MAX17043_COMMAND_POR, MAX17043_COMMAND);
}

/****************************************************************************************
// Function      : Get compensation value 
// Description   : Get the ModelGauge compensation value - an obscure 8-bit value set to 0x97 by default.
// Parameters    : none
// Return        : 8-bit value read from the CONFIG register's MSB.
****************************************************************************************/
uint8_t GetBatGaugeCompensation()
{
	uint16_t ucConfigReg = ReadFromGauge(MAX17043_CONFIG);
	uint8_t ucCompensation = (ucConfigReg & 0xFF00) >> 8;

	return ucCompensation;
}

/****************************************************************************************
// Function      : Get the configuration registers of the gauge
// Description   : Read the 16-bit value of the CONFIG Register.
// Parameters    : none
// Return        : 16-bit integer value representing the msb and lsb bytes of the CONFIG register.
****************************************************************************************/
uint16_t GetBatGaugeConfigRegister()
{
	return ReadFromGauge(MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : Get gauge compenstaion value
// Description   : Set the 8-bit compensation value. This
                 : is an obscure 8-bit value that has some effect on Maxim's ModelGauge algorithm.
// Parameters    :  Should be a value between 0-255.
// Return        : 0 on success, positive integer on fail
****************************************************************************************/
uint8_t SetBatGaugeCompensation(uint8_t ucNewCompensation)
{
	/*
		The CONFIG register compensates the ModelGauge algorith. The upper 8 bits
		of the 16-bit register control the compensation.
		Read the original configReg, so we can leave the lower 8 bits alone	:
	*/

	uint16_t configReg = ReadFromGauge(MAX17043_CONFIG);

	/* Mask out compensation bits */
	configReg &= 0x00FF;
	configReg |= ((uint16_t)ucNewCompensation << 8) | configReg;

	return WriteGauge(configReg, MAX17043_CONFIG);
}

/****************************************************************************************
// Function      : Write function to battery gauge 
// Description   : Write 16 bits to the requested address. After
                 : writing the address to be written, two sequential 8-bit writes will occur.
                 : the msb is written first, then lsb.
// Parameters    : [data] - A 16-bit integer to be written to the device.
                 : [ucAddress] - An 8-bit address to be written to.
// Return        : 0 on success, positive integer on fail.
****************************************************************************************/
uint8_t WriteGauge(uint16_t data, uint8_t ucAddress)
{
	uint8_t ucMsb = 0;
	uint8_t ucLsb = 0;

	ucMsb = (data & 0xFF00) >> 8;
	ucLsb = (data & 0x00FF);
	uint8_t txData[3] = { ucAddress, ucMsb, ucLsb };
	return HAL_I2C_Master_Transmit(&hi2c3, MAX17043_ADDRESS << 1, txData, 3, BAT_GUAGE_DELAY);
}

/****************************************************************************************
// Function      : Read function from battery gauge
// Description   : Read 16-bits from the requested address of a device.
// Parameters    : An 8-bit address to be read from.
// Return        : A 16-bit value read from the device's address will be returned.
****************************************************************************************/
uint16_t ReadFromGauge(uint8_t address)
{
	uint8_t ucRxData[2] = {0};

	HAL_I2C_Master_Transmit(&hi2c3, MAX17043_ADDRESS << 1, &address, 1, BAT_GUAGE_DELAY);
	HAL_I2C_Master_Receive(&hi2c3, MAX17043_ADDRESS << 1, ucRxData, 2, BAT_GUAGE_DELAY);

	return ((uint16_t)((uint8_t)ucRxData[0] << 8) | (uint8_t)ucRxData[1]);//check here

	//return ((uint16_t)(ucRxData[0] << 8 | ucRxData[1]));
}

#if 0
uint16_t ReadFromGauge(uint8_t address)
{
  uint8_t msb, lsb;
  int16_t timeout = 1000;

  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(address);
  Wire.endTransmission(false);

  Wire.requestFrom(MAX17043_ADDRESS, 2);
  while ((Wire.available() < 2) && (timeout-- > 0))
    delay(1);
  msb = Wire.read();
  lsb = Wire.read();

  return ((uint16_t) msb << 8) | lsb;
}

uint8_t WriteGauge(uint16_t data, uint8_t address)
{
  uint8_t msb, lsb;
  msb = (data & 0xFF00) >> 8;
  lsb = (data & 0x00FF);
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(address);
  Wire.write(msb);
  Wire.write(lsb);
  return (Wire.endTransmission());
}

void MAX17043_Init()
{
	uint8_t config_data[] = {0x00, 0x80}; // Configuring MAX17043, refer to datasheet

	HAL_I2C_Master_Transmit(&hi2c1, MAX17043_ADDRESS, config_data, sizeof(config_data), HAL_MAX_DELAY);
}

uint16_t MAX17043_ReadVoltage()
{
	uint8_t data[2];

	HAL_I2C_Mem_Read(&hi2c1, MAX17043_ADDRESS, MAX17043_VCELL, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	uint16_t voltage = ((uint16_t)data[0] << 8) | data[1];

	return voltage;
}

float MAX17043_ReadSOC()
{
	uint8_t data[2];

	HAL_I2C_Mem_Read(&hi2c1, MAX17043_ADDRESS, MAX17043_SOC, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	uint16_t soc_raw = ((uint16_t)data[0] << 8) | data[1];
	float soc = soc_raw / 256.0;

	return soc;
}

uint8_t WriteGauge(uint16_t data, uint8_t address)
{
	uint8_t msb, lsb;

	msb = (data & 0xFF00) >> 8;
	lsb = (data & 0x00FF);

	uint8_t txData[3] = { address, msb, lsb };

	return HAL_I2C_Master_Transmit(&hi2c1, MAX17043_ADDRESS << 1, txData, 3, HAL_MAX_DELAY);
}

uint16_t ReadFromGauge(uint8_t address)
{
	uint8_t rxData[2];

	HAL_I2C_Master_Transmit(&hi2c1, MAX17043_ADDRESS << 1, &address, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, MAX17043_ADDRESS << 1, rxData, 2, HAL_MAX_DELAY);

	return ((uint16_t)rxData[0] << 8) | rxData[1];
}

// Define a static MAX17043 object called lipo, which we'll use in the sketches.
//MAX17043 lipo;
#endif
