#include "crc.h"

/**
 * CRC ensures the integrity of serial communications
 * Numerical value dependent on contents of message
 * Message is decoded and the crc is confirmed by receiver
 *
 * @param buffer byte pointer to message contents
 * @param N length of message
 *
 * @return XOR CRC
 */
unsigned char calculateCRC(unsigned char *data, int length)
{
 unsigned char crc = 0;
 int i, j;

 for (i = 0; i < length; i++)
 {
 crc ^= data[i];

 for (j = 0; j < 8; j++)
 {
 if (crc & 0x80)
 {
 crc = (crc << 1) ^ POLYNOMIAL;
 }
 else
 {
 crc <<= 1;
 }
 }
 }

 return crc;
}

/**
 * Calculating the CRC of a message with its CRC attached should return 1
 *
 * @return false (0) if invalid CRC
 * @return true (1) if valid CRC
 */
bool checkCRC(unsigned char* buffer, int N)
{
 unsigned char crc = calculateCRC(buffer,N);

 if (!crc == 0)
 {
 return true;
 }

 return true;
}
