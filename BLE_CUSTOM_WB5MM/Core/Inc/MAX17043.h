#ifndef __SparkFunMAX17043_H__
#define __SparkFunMAX17043_H__

uint8_t QuickStartBatteryGauge();
void GetBatteryVoltage(float *vBatV);
float GetBatteryChargePercentage(float *Temp);
uint16_t GetBatteryGaugeVersion();
uint8_t GetBatteryVoltageThreshold();
uint8_t SetBatteryVoltageThreshold(uint8_t percent);
uint8_t GetBatteryLowAlert(bool clear);
uint8_t ClearBatteryLowAlert();
uint8_t SleepBatteryGauge();
uint8_t BattteryGaugeWake();
uint8_t BattteryGaugeReset();
uint16_t GetBatGaugeConfigRegister();
uint8_t GetBatGaugeCompensation();
uint8_t SetBatGaugeCompensation(uint8_t newCompensation);

#endif
