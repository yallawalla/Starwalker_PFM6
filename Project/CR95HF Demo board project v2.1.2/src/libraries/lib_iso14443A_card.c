/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : lib_iso14443A_card.c
* Author             : MMY Application Team
* Version            : 1.0
* Date               : 20/12/2011
* Description        : Manage the iso14443A communication in the card mode emulator.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "lib_iso14443A.h"
#include "lib_iso14443A_card.h"
//#include "main.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
#include "hw_config.h"
#include "stm32f10x.h"

extern CARDEMULATOR						CurrentCardEmulator;

/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup ISO14443A
 * 	@{
 */


static int8_t CardLibA_GetCommandId (uint8_t *pData); 

/**
 * @brief  this function initializes the global structure that defines the card emulator 
 * @param  void
 * @return void
 */
void CardLibA_InitializeStructure( void )
{
   CurrentCardEmulator.State = CARDSTATE_DESACTIVATED;
   CurrentCardEmulator.Protocol = CARDPROTOCOL_UNKNOWN;
}


/**
 * @brief  this function reads from the interface bus the command sent by the reader and returns a response
 * @param  void
 * @return void
 */
void CardLibA_ReceiveCommand( void )
{
uint8_t pData[MAX_BUFFER_SIZE];
int8_t 	status;

	errchk(SPIUART_PoolingReading (pData));
	// field cut off
	if (pData[0] == 0x8F && pData[1] == 0x00)
	{	
		CurrentCardEmulator.State = CARDSTATE_NO_RFFIELD;	
		// desactivate IRQ In 
		Interrupts_ExternalIrq_Desactivate( );	

	}
	else 
	{
		CardLibA_DecodeCommand (pData);
		CR95HF_Listen(pData);
	}
	// Enable Interrupt on the falling edge on the IRQ pin of CR95HF 
	EXTI_ClearITPendingBit(EXTI_CR95HF_LINE);
	EXTI->IMR |= EXTI_CR95HF_LINE;
Error:
	;
}


/**
 * @brief  this function reads from the interface bus the command sent by the reader and returns a response
 * @param  void
 * @return void
 */
int8_t CardLibA_DecodeCommand( uint8_t *pData )
{
	if (pData[CR95HF_COMMAND_OFFSET] != LISTEN_RESULTSCODE_OK)
		return ERRORCODE_GENERIC;

	switch (pData[CR95HF_DATA_OFFSET])
	{
		case COMMAND_RATS: 
			CardLibA_RATS ( pData );
		break;

		case COMMAND_PPS: 
//			CardLibA_PPS ( pData );
		break;
		

		case COMMAND_IBLOCK02:
		case COMMAND_IBLOCK03:
			
			switch (CardLibA_GetCommandId(pData))
			{
				
				case CARDPROTOCOL_SELECTCMD_SELECTAPPLICATION: 
					 CardLibA_SelectApplication( pData );		
				break;		
				case CARDPROTOCOL_SELECTCMD_SELECTCCFILE: 
					CardLibA_SelectCCfile( pData );	
				break;				
							
				case CARDPROTOCOL_SELECTCMD_SELECTNDEFFILE: 
					CardLibA_SelectNDEFfile( pData );	
				break;
				case CARDPROTOCOL_SELECTCMD_UNKNOWN: 
					CardLibA_SendAnErrorCode( pData, LIB14443ACARD_NFCTYPE4_APPLICATIONNOTFOUND );	
				break;
			}

		break;

		default:
			return LIB14443ACARD_ERRORCODE_COMMANDUNKNOWN; 

	
	}
	
	return LIB14443ACARD_SUCESSSCODE; 
	
}

/**
 * @brief  this function decodes and returns a response to RATS command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_RATSCOMMAND	: CR95HF returns an error code
 */
int8_t CardLibA_RATS( uint8_t *pData )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;

	if (pData[CR95HF_DATA_OFFSET] != COMMAND_RATS)
		return LIB14443ACARD_ERRORCODE_CODECOMMAND;  
	
	// save the RF commmand parameters
	ISO14443A_Card.CID = pData[DATA_OFFSET+1] & CID_MASK; 	
	ISO14443A_Card.FSDI = (pData[DATA_OFFSET+1] & FSDI_MASK)>>4; 

	// creates the card response 
	//06 75 77 81 02 80 02F0
	DataToToEmit [Length++] = LIB14443ACARD_TL;	   		// Tl : number of bytes
	DataToToEmit [Length++] = LIB14443ACARD_T0;	   		// T0 : format
	DataToToEmit [Length++] = LIB14443ACARD_TA1;	    // TA1 : datarates 
	DataToToEmit [Length++] = LIB14443ACARD_TB1;	    // TB1 : timing
	DataToToEmit [Length++] = LIB14443ACARD_TC1;	    // TC1 : interface

	DataToToEmit [Length++] = 0x80;	    // T1 : historical byte

	// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	 
		
	// emits the response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_RATSCOMMAND;

}

/**
 * @brief  this function decodes and returns a response to RATS command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_RATSCOMMAND	: CR95HF returns an error code
 */
 /*
int8_t CardLibA_PPS( uint8_t *pData )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;

	if (pData[DATA_OFFSET] != COMMAND_PPS)
		return LIB14443ACARD_ERRORCODE_CODECOMMAND;  
	

	// creates the card response 
	//06 75 77 81 02 80 02F0
	DataToToEmit [Length++] = LIB14443ACARD_TL;	   		// Tl : number of bytes
	DataToToEmit [Length++] = LIB14443ACARD_T0;	   		// T0 : format
	DataToToEmit [Length++] = LIB14443ACARD_TA1;	    // TA1 : datarates 
	DataToToEmit [Length++] = LIB14443ACARD_TB1;	    // TB1 : timing
	DataToToEmit [Length++] = LIB14443ACARD_TC1;	    // TC1 : interface

	DataToToEmit [Length++] = 0x80;	    // T1 : historical byte

	// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	 
		
	// emits the response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_RATSCOMMAND;

}
 */
/**
 * @brief  this function decodes and returns a response to Select Application command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_SENDERRORCODE : CR95HF returns an error code
 */
int8_t CardLibA_SendAnErrorCode(  uint8_t *pData,uint16_t ErrorCode )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;
	 	
	// creates the card response 
	// add the IBklock
	DataToToEmit [Length++] = pData[CR95HF_DATA_OFFSET];
	// add the command status
	DataToToEmit [Length++] = ((ErrorCode && 0xFF00) >> 8);
	DataToToEmit [Length++] = ErrorCode && 0x00FF;

	// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	

	// emits the card response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_SENDERRORCODE;
}

/**
 * @brief  this function decodes and returns a response to Select Application command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_SELECTAPPLICATION : CR95HF returns an error code
 */
int8_t CardLibA_SelectApplication( uint8_t *pData )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;
	 	
	// creates the card response 
	// add the IBklock
	DataToToEmit [Length++] = pData[DATA_OFFSET];
	// add the command status
	DataToToEmit [Length++] = ((LIB14443ACARD_NFCTYPE4_STATUSOK & 0xFF00) >> 8);
	DataToToEmit [Length++] = LIB14443ACARD_NFCTYPE4_STATUSOK & 0x00FF;

		// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	

	// emits the card response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_SELECTAPPLICATION;
}

/**
 * @brief  this function decodes and returns a response to Select CCfile command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_SELECTCCFILE : CR95HF returns an error code
 */
int8_t CardLibA_SelectCCfile( uint8_t *pData )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;
	 	
	// creates the card response 
	// add the IBklock
	DataToToEmit [Length++] = pData[DATA_OFFSET];
	// add the command status
	DataToToEmit [Length++] = ((LIB14443ACARD_NFCTYPE4_STATUSOK & 0xFF00) >> 8);
	DataToToEmit [Length++] = LIB14443ACARD_NFCTYPE4_STATUSOK & 0x00FF;
	// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	
	// emits the card response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_SELECTCCFILE;
}

/**
 * @brief  this function decodes and returns a response to Select CCfile command
 * @param  pData : RF command received by CR95HF
 * @return 	LIB14443ACARD_SUCESSSCODE : function succesful
 * @return 	LIB14443ACARD_ERRORCODE_CODECOMMAND : the command code doesn't match with this function
 * @return 	LIB14443ACARD_ERRORCODE_SELECTNDEFFILE : CR95HF returns an error code
 */
int8_t CardLibA_SelectNDEFfile( uint8_t *pData )
{
uint8_t Length =0,
		pResponse [SENDRESPONSE_BUFFER_SIZE],
		DataToToEmit [ATS_BUFFER_SIZE];
int8_t 	status;
	 	
	// creates the card response 
	// add the IBklock
	DataToToEmit [Length++] = pData[CR95HF_DATA_OFFSET];
	// add the command status
	DataToToEmit [Length++] = ((LIB14443ACARD_NFCTYPE4_STATUSOK & 0xFF00) >> 8);
	DataToToEmit [Length++] = LIB14443ACARD_NFCTYPE4_STATUSOK & 0x00FF;
	// control byte append CRC + 8 bits
	DataToToEmit [Length++] = SEND_MASK_APPENDCRC | SEND_MASK_8BITSINFIRSTBYTE;	
	// emits the card response
	errchk(CR95HF_Send (Length,DataToToEmit,pResponse));

	return LIB14443ACARD_SUCESSSCODE ;
Error:
	return LIB14443ACARD_ERRORCODE_SELECTNDEFFILE;
}

/**
 * @brief  this function returns the idenfier of the received command
 * @param  pData : RF command received by CR95HF
 * @return 	CARDPROTOCOL_SELECTCMD_SELECTCCFILE : the receiving command is a Select CC file 
 * @return 	CARDPROTOCOL_SELECTCMD_SELECTAPPLICATION : the receiving command is a Select Application
 * @return 	CARDPROTOCOL_SELECTCMD_SELECTNDEFFILE	: the receiving command is a Select NDEF file
 * @return 	CARDPROTOCOL_SELECTCMD_UNKNOWN	: Command unknown
 */
static int8_t CardLibA_GetCommandId (uint8_t *pData)
{
uint16_t FileIdentifier=0x0000;

	if (pData[CR95HF_DATA_OFFSET+1] != LIB14443ACARD_NFCTYPE4_CLASS)
		return LIB14443ACARD_ERRORCODE_COMMANDUNKNOWN;

	if (pData[CR95HF_DATA_OFFSET+2] == LIB14443ACARD_NFCTYPE4_SELECTCMD)
	{
		FileIdentifier = (uint16_t) ( ((pData[CR95HF_DATA_OFFSET+6])<<8) | (pData[CR95HF_DATA_OFFSET+7]) ); 
		// catch the file idenfifier
		switch (FileIdentifier)
		{
			case LIB14443ACARD_NFCTYPE4_CCFILE: 
				return CARDPROTOCOL_SELECTCMD_SELECTCCFILE;
			
			case LIB14443ACARD_NFCTYPE4_NDEFFILE: 
				return CARDPROTOCOL_SELECTCMD_SELECTAPPLICATION;

			case LIB14443ACARD_NFCTYPE4_NDEFAPPLICATION: 
				return CARDPROTOCOL_SELECTCMD_SELECTNDEFFILE;

			default : 
				return CARDPROTOCOL_SELECTCMD_UNKNOWN;
		}
	}
		
	 
	return LIB14443ACARD_ERRORCODE_COMMANDUNKNOWN;		

}



