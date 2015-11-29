/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso14443SR.c
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
#include "stm32f10x.h"
#include "lib_iso14443SR.h"
#include "miscellaneous.h"
//#include "main.h"
/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup ISO14443_SR
 * 	@{
 */

 /* --------------------------------------------------
 * ISO 14443SR commands -- CMD | Length | data
 * -------------------------------------------------- */

static uc8 INIT_TYPE_SR[]    = {PROTOCOL_SELECT, 					  // Cmd Seolct protocole				
										0x02,				  // Length
										0x03,				  //
										0x01};


static uc8 INITIATE[]	      = {SEND_RECEIVE, 						 // Cmd	Send Reiceive			
									 0x02,					 // Length
									 0x06,					 // Initiate
									 0x00};					 //



static uc8 SELECT_CHIP_ID[]  = {SEND_RECEIVE, 						 // Cmd	Send Reiceive			
									 0x02,					 // Length
									 0x0E,					 								
									 0x00};					 // Chip_UI



static uc8 GET_UID[]        = {SEND_RECEIVE, 						 // Cmd	Send Reiceive			
									 0x01,					 // Length
									 0x0B};				     //Get UID								
									 					 


static uc8 READ_BLOCK[]      = {SEND_RECEIVE, 						 // Cmd	Send Reiceive			
									 0x02,					 // Length
									 0x08,					 								
									 0x00};					 



static uc8 WRITE_BLOCK[]      = {SEND_RECEIVE, 				     // Cmd	Send Reiceive			
									 0x06,					 // Length
									 0x09,
									 0x00,
									 0x23,
									 0x05,
									 0x19,	 								
									 0x84};					 


static uc8 CARD_PRESENT1[]       = {SEND_RECEIVE,				  //select chip
										0x01,
										0x0B};

static uc8 EchoCmd[2] = {0x55,0x00};

int8_t ISO14443_Ex14443(uc8 *pCommand, uint8_t *pResponse)
{
	uint8_t ResBuffer[256];
	uint8_t CmdCode, CmdLength;
	uint8_t ResCode, ResLength;
	
	/* Recover the command code and the length from the command buffer */
	CmdCode 	 = pCommand[CR95HF_COMMAND_OFFSET];
	CmdLength    = pCommand[CR95HF_LENGTH_OFFSET];

	/* Check any problems concerning the buffer structure */
	if(CmdCode == 0x55 && CmdLength != 0x00)
		return ERRORCODE_GENERIC;

	if(CmdCode != 0x55 && CmdLength == 0x00)
		return ERRORCODE_GENERIC;

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
		return ERRORCODE_GENERIC;

	if(CmdCode == SEND_RECEIVE && ResCode != 0x87)
		return ERRORCODE_GENERIC;


	if(CmdCode != 0x55 && CmdCode != SEND_RECEIVE && ResCode != 0x00)
		return ERRORCODE_GENERIC;

	
	return RESULTOK;
}

/* --------------------------------------------------------------
 * ISO14443_Ex14443_3   type A , B, and SR
 * @brief : Exchange ISO14443-3 
 *          data with the card using CR95HF
 *
 * -------------------------------------------------------------- */
int8_t ISO14443_Ex14443_3(uc8 *pCommand, uint8_t *pResponse)
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

	if(CmdCode == SEND_RECEIVE && ResCode != 0x80)
	{
		return ERRORCODE_GENERIC;
	}

	if(CmdCode != 0x55 && CmdCode != SEND_RECEIVE && ResCode != 0x00)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}


int8_t ISO14443SR_Init(uint8_t TransmissionDataRate, uint8_t ReceptionDataRate)
{
	uint8_t DataRead[MAX_BUFFER_SIZE];

	if(ISO14443_Ex14443_3(EchoCmd, DataRead) == ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	if(ISO14443_Ex14443_3(INIT_TYPE_SR, DataRead) == ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	return RESULTOK;
}


/*Initiate*/
int8_t ISO14443SR_Initiate( uint8_t *pDataRead )
{
	if(ISO14443_Ex14443_3(INITIATE, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}


/*Select_Chip_ID*/
int8_t ISO14443SR_Select_Chip_ID( uint8_t *pDataRead, uint8_t *Chip_ID )
{
   	uint8_t DataToSend[MAX_BUFFER_SIZE];
	memcpy(DataToSend    , SELECT_CHIP_ID, MAX_BUFFER_SIZE);
	memcpy(&DataToSend[3], Chip_ID    ,1);
	if(ISO14443_Ex14443_3(DataToSend, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	return RESULTOK;
}


/*Get UID*/
int8_t ISO14443SR_GetUID( uint8_t *pDataRead )
{
uint8_t Tag_error_check;

	if(ISO14443_Ex14443_3(GET_UID, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	Tag_error_check = pDataRead[OFFSET_LENGTH]+1;
	if(pDataRead[Tag_error_check] & CRC_MASK == CRC_ERROR_CODE)
	return ERRORCODE_GENERIC;
	

	return RESULTOK;
}

int8_t ISO14443SR_ReadBlock( uint8_t *pDataRead )
{
	if(ISO14443_Ex14443_3(READ_BLOCK, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}


int8_t ISO14443SR_WriteBlock( uint8_t *pDataRead )
{
	if(ISO14443_Ex14443(WRITE_BLOCK, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}

int8_t ISO14443SR_IsPresent( void )
{
	uint8_t DataRead[MAX_BUFFER_SIZE];

	/* Initializing buffer */
	memset(DataRead,0,MAX_BUFFER_SIZE);

	/* Init the ISO14443 TypeSR communication */
	if(ISO14443SR_Init(0,0)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	/* Inventory */
	if(ISO14443SR_Initiate(DataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	/* An ISO14443 card is in the field */	
  return RESULTOK;
}


int8_t ISO14443SR_Anticollision(char *message)
{
	uint8_t DataRead[MAX_BUFFER_SIZE],Chip_ID[4];

	/* REQ B attempt */
	if(ISO14443SR_Initiate(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	
	ISO14443SR_DisplayINITIATE(message, DataRead);

	 memcpy(Chip_ID,&DataRead[CR95HF_DATA_OFFSET],1);
	 if(ISO14443SR_Select_Chip_ID(DataRead, Chip_ID)== ERRORCODE_GENERIC)
		 return ERRORCODE_GENERIC;
	 
	 ISO14443SR_DisplaySELECT_CHIP_ID(message, DataRead);

	 if(ISO14443SR_GetUID(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	 
	 ISO14443SR_DisplayGET_UID(message, DataRead);

	 if(ISO14443SR_ReadBlock(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	  
	  ISO14443SR_DisplayREAD_BLOCK(message, DataRead);

	 if(ISO14443SR_WriteBlock(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	  
	  ISO14443SR_DisplayWRITE_BLOCK(message, DataRead);


  return RESULTOK;

}

int8_t ISO14443SR_CardTest( void )
{
	uint8_t DummyBuffer[MAX_BUFFER_SIZE];	
	/*uint8_t DataToSend[MAX_BUFFER_SIZE];
	memcpy(DataToSend    , CARD_PRESENT1, MAX_BUFFER_SIZE);
	memcpy(&DataToSend[3], CARD_PRESENT1    ,1);
	if(!ISO14443_Ex14443_3(DataToSend, DummyBuffer)) */
	if(ISO14443_Ex14443_3(CARD_PRESENT1, DummyBuffer)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
		return RESULTOK;
}

void ISO14443SR_DisplayINITIATE(char *message, uint8_t *pData)
{
	uint8_t i;

	sprintf(message, "%s..TAG_SR detected !\n",message);
	sprintf(message, "%s Initiate = ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		if(i == 9) 
		{
			sprintf(message, "%s\n ", message);
		}			
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}
}

void ISO14443SR_DisplaySELECT_CHIP_ID( char *message, uint8_t *pData )
{
	uint8_t i;
	
    sprintf(message, "%s\n ", message);
	sprintf(message, "%s Chip_ID = ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}
	sprintf(message, "%s\n ", message);
}


void ISO14443SR_DisplayGET_UID( char *message, uint8_t *pData )
{
	uint8_t i,DataRead[MAX_BUFFER_SIZE],ID[ISO144443SR_NBBYTE_UID];
	
	sprintf(message, "%sGet_UID = ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}

	sprintf(message, "%s\n ", message);
	sprintf(message, "%sUID=",message);
	if(ISO14443SR_GetUID(DataRead))
	{
	memcpy(ID+2,&DataRead[CR95HF_DATA_OFFSET],8);
	for(i=0;i<ISO144443SR_NBBYTE_UID;i++)
	{
		sprintf(message, "%s%02X",message, ID[CR95HF_DATA_OFFSET + i]);
	}
	}
}


void ISO14443SR_DisplayREAD_BLOCK( char *message, uint8_t *pData )
{
	uint8_t i;
	
    sprintf(message, "%s\n ", message);
	sprintf(message, "%sRead_Block = ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}
}

void ISO14443SR_DisplayWRITE_BLOCK( char *message, uint8_t *pData )
{
	uint8_t i;
	
    sprintf(message, "%s\n ", message);
	sprintf(message, "%sWrite_Block = ",message);

	for(i=0;i<pData[CR95HF_LENGTH_OFFSET];i++)
	{
		sprintf(message, "%s%02X",message, pData[CR95HF_DATA_OFFSET + i]);
	}
}



///* ------------------------------------------------------------------------------------------ 
// *	CR95HF_delay
// * 	Debug tool
// ----------------------------------------------------------------------------------------- */
//void STRF_delay1(uint16_t t)
//{	
//	for(;t>0;t--)
//	{
//		__NOP();
//	}
//}
//
