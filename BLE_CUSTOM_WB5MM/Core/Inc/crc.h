#ifndef CRC_H_FUNCTIONS
#define CRC_H_FUNCTIONS


#include<stdbool.h>


#define POLYNOMIAL 0xD8;


bool checkCRC(unsigned char* buffer, int N);

unsigned char calculateCRC(unsigned char *data, int length);

#endif
