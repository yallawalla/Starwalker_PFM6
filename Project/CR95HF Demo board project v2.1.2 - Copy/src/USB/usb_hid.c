/**
  ******************************************************************************
  * @file    usb_hid.c 
  * @author  MMY Application Team
  * @version V2.0
  * @date    12/06/2012
  * @brief   Handle messages received through USB HID
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_hid.h"
#include "usb_lib.h"
#include "string.h"
#include "miscellaneous.h"
#include "lib_CR95HF.h"

#include "usb_bot.h"

//#include "ff.h"

#include "stm32f10x_gpio.h"
#include "hw_config.h"

/** @addtogroup USB
 * 	@{
 */

/** @addtogroup HID
  * @{
  */
extern bool HID_TRANSACTION;	
/**  
 * @brief this function is blank function
 * @param  none
 * @retval None
 */
void HID_Data_In( void )
{
	/* None. */
}

/**  
 * @brief this function is the input point of HID user command. the user command is exeuted and a response is send to the user through USB bus.
 * @param  none
 * @retval CR95HF_SUCCESS_CODE
 */
int8_t HID_Data_Out( void )
{
uint8_t 	RcvBuffer 	[HID_MAX_BUFFER_SIZE], //0x3F bytes reserved for data + 1 for report ID
 			SendBuffer	[CR95HF_RESPONSEBUFFER_SIZE]; //0x3F bytes reserved for data + 1 for report ID

  	
	HID_TRANSACTION = TRUE;

	memset 	(RcvBuffer,		0x00,HID_MAX_BUFFER_SIZE);
	memset 	(SendBuffer,	0x00,HID_MAX_BUFFER_SIZE);

	/* Read received data (64 bytes) ... */
	SetEPRxStatus(ENDP3, EP_RX_VALID);  
	USB_SIL_Read(EP3_OUT, RcvBuffer);

	// Rcv buffer = command to CR95HF
	// SendBuffer = CR95HF response


	switch(RcvBuffer[HID_OFFSET])
	{
		// Send CR95HF command to CR95HF
		case HID_SPI_SEND_CR95HFCMD : 
			if (RcvBuffer[HID_OFFSET_CMDCODE] != BAUD_RATE) 
				SPIUART_SendReceive(&RcvBuffer[HID_OFFSET_CMDCODE], &SendBuffer[HID_OFFSET_CMDCODE]);
			else // UART baud rate is a specific command because MCU and CR95HF has to modify their configurations
				CR95HF_ChangeUARTBaudRate (&RcvBuffer[HID_OFFSET_CMDCODE], &SendBuffer[HID_OFFSET_CMDCODE]);	
		break;
		// send customs command
		case HID_SPI_SEND_DATA:				
			CR95HF_SendCustomCommand(&RcvBuffer[HID_OFFSET_CMDCODE], &SendBuffer[HID_OFFSET_CMDCODE]);					
		break;
	}
		
	// the baudrate command are an exeption and shall be threat seperately
	if (RcvBuffer[HID_OFFSET_CMDCODE] == BAUD_RATE) 
	{
		// the baud rate command has been emmiting 
		UART_Initialization(BAUDRATE_DATARATE_DEFAULT );			
	}



	/* SendBuffer format
		1st byte 	: ID_SEND_HID_RESPONSE
		2nd byte 	: repply or error code flag 
		3rd byte 	: nb byte
		others bytes: data
	*/

	SendBuffer[HID_OFFSET] = HID_SEND_HID_RESPONSE;
	SendBuffer[HID_OFFSET_LENGTH] =MIN(HID_MAX_BUFFER_SIZE,SendBuffer[HID_OFFSET_LENGTH]);

	/* Allows the transmission */
	SetEPTxStatus(ENDP3, EP_TX_VALID);
	USB_SIL_Write(EP3_IN, SendBuffer, HID_MAX_BUFFER_SIZE);


	return CR95HF_SUCCESS_CODE;
}

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/


