/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso18092c
* Author             : MMY Application Team
* Version            : V1.0.0
* Date               : 03/01/2010
* Description        : 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "lib_iso18092.h"
#include "miscellaneous.h"
#include "main.h"

/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup ISO18092
 * 	@{
 */

/* CR95HF command (Allows you to check the STRF status) */
static uc8 EchoCmd[2] = {0x55,0x00};

//extern uc8 ECHO[];

int8_t ISO18092_INITIALISATION(uc8 *pCommand, uint8_t *pResponse)
{
	uint8_t ResBuffer[256];
	uint8_t CmdCode, CmdLength;
	uint8_t ResCode, ResLength;
	
	/* Recover the command code and the length from the command buffer */
	CmdCode 	 = pCommand[CR95HF_COMMAND_OFFSET];
	CmdLength    = pCommand[CR95HF_LENGTH_OFFSET];

	/* Check any problems concerning the buffer structure */
	if(CmdCode == 0x55 && CmdLength != 0x00)
	{
		return ERRORCODE_GENERIC;
	}
	if(CmdCode != 0x55 && CmdLength == 0x00)
	{
		return ERRORCODE_GENERIC;
	}

	/* Send the buffer on CR95HF (the response is in the buffer ResBuffer) */
	memset(ResBuffer, 0x00, 256);
	SPIUART_SendReceive(pCommand, ResBuffer);
	
	/* Recover the command code and the length from the response buffer */
	ResCode  	= ResBuffer[CR95HF_COMMAND_OFFSET];
	ResLength   = ResBuffer[CR95HF_LENGTH_OFFSET];

	/* Copy the whole buffer in the response buffer */
	memcpy(pResponse, ResBuffer, ResLength + 2);

	/* Check any errors in the response */
	if(CmdCode == 0x55 && ResCode != 0x55)
	{
		return ERRORCODE_GENERIC;
	}
	
	if(CmdCode == SEND_RECEIVE && ResCode == 0x80 )  
	{
	  	return ERRORCODE_GENERIC;
	}
	
	if(CmdCode != 0x55 && CmdCode != SEND_RECEIVE && ResCode != 0x00)
	{
	   	return RESULTOK;
	}
	   
	return RESULTOK;
}

int8_t ISO18092_Init(uint8_t TransmissionDataRate, uint8_t ReceptionDataRate)
{
	uint8_t DataRead[MAX_BUFFER_SIZE];
	if(!ISO18092_INITIALISATION(EchoCmd, DataRead))
	{
		return ERRORCODE_GENERIC;
	}
	if(!ISO18092_INITIALISATION(INIT_TYPE_F, DataRead))
	{
		return ERRORCODE_GENERIC;
	}
	return RESULTOK;
}



int8_t ISO18092_ReqC( uint8_t *pDataRead )
{
	if(!ISO18092_INITIALISATION(REQC, pDataRead))
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}



int8_t ISO18092_IsPresent( void )
{
	uint8_t DataRead[MAX_BUFFER_SIZE];

	/* Initializing buffer */
	memset(DataRead,0,MAX_BUFFER_SIZE);

	/* Init the ISO18092 communication */
	if(!ISO18092_Init(0,0))
	{
		return ERRORCODE_GENERIC;
	}

	/* WakeUp attempt */
	if(!ISO18092_ReqC(DataRead))
	{
		return ERRORCODE_GENERIC;
	}
	
	/* An ISO14443 card is in the field */	
  return RESULTOK;
}


int8_t ISO18092_Anticollision(char *message)
{
	uint8_t DataRead[MAX_BUFFER_SIZE]; 
	/*,ID1[8],ID2[8];*/

	/* REQ A attempt */
	if(!ISO18092_ReqC(DataRead))
	{
		return ERRORCODE_GENERIC;
	}
	ISO18092_DisplayREQC(message, DataRead);
	
	/* Anticollision process done ! */	
  return RESULTOK;

}


int8_t ISO18092_CardTest( void )
{
	uint8_t DummyBuffer[MAX_BUFFER_SIZE];

		if(!ISO18092_INITIALISATION(CARD_PRESENT4, DummyBuffer))
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}
 
void ISO18092_DisplayREQC(char *message, uint8_t *pData)
{
	uint8_t i;

	sprintf(message, "%s..Felica detected !\n",message);
	sprintf(message, "%s ATQC = \n ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		if(i == 9) 
		{
			sprintf(message, "%s\n ", message);
		}			
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}
	sprintf(message, "%s\n ", message);
}


//
//
///* ------------------------------------------------------------------------------------------ 
// *	CR95HF_delay
// * 	Debug tool
// ----------------------------------------------------------------------------------------- */
//void STRF_delay4(uint16_t t)
//{	
//	for(;t>0;t--)
//	{
//		__NOP();
//	}
//}

