
#include "uart.h"
#include <stdbool.h>

extern UART_HandleTypeDef huart1;
/**
 * @brief   Receives data from UART.
 * @param   *data: Array to save the received data.
 * @param   length:  Size of the data.
 * @return  status: Report about the success of the receiving.
 */
uart_status uart_receive(uint8_t *data, uint16_t length)
{
  uart_status status = UART_ERROR;

  if (HAL_OK == HAL_UART_Receive(&huart1, data, length, UART_TIMEOUT))
  {
    status = UART_OK;
  }
#if 0
  blUartIntrFlag = false;
  HAL_UART_Receive_IT(&huart1, data, length);
  while(!blUartIntrFlag)
  {
	  HAL_Delay(10);
	  if (10 == ucTimeout++)
	  {
		  status = UART_ERROR;
		  break;
	  }
  }
  if(blUartIntrFlag)
  {
	  status = UART_OK;
  }
#endif
  return status;
}

/**
 * @brief   Transmits a string to UART.
 * @param   *data: Array of the data.
 * @return  status: Report about the success of the transmission.
 */
uart_status uart_transmit_str(uint8_t *data)
{
  uart_status status = UART_ERROR;
  uint16_t length = 0u;

  /* Calculate the length. */
  while ('\0' != data[length])
  {
    length++;
  }

  if (HAL_OK == HAL_UART_Transmit(&huart1, data, length, UART_TIMEOUT))
  {
    status = UART_OK;
  }

  return status;
}

/**
 * @brief   Transmits a single char to UART.
 * @param   *data: The char.
 * @return  status: Report about the success of the transmission.
 */
uart_status uart_transmit_ch(uint8_t data)
{
  uart_status status = UART_ERROR;

  /* Make available the UART module. */
  if (HAL_UART_STATE_TIMEOUT == HAL_UART_GetState(&huart1))
  {
    HAL_UART_Abort(&huart1);
  }

  if (HAL_OK == HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT))
  {
    status = UART_OK;
  }
  return status;
}
