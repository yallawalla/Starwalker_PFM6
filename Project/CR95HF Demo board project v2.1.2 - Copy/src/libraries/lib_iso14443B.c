/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso14443B.c
* Author             : SMD Application Team
* Version            : 1.0
* Date               : 18/05/2010
* Description        : Manage the iso14443B communication.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "lib_iso14443B.h"
#include "string.h"
#include "stdio.h"
#include "miscellaneous.h"
#include "hw_config.h"
//#include "main.h"

/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup ISO1443B
 * 	@{
 */

ISO14443B_CARD 	ISO14443B_Card;

/* --------------------------------------------------
 * code templates for ISO14443B protocol
 * command =  Command code | Length | data(Le)
 * -------------------------------------------------- */
//uc8 INIT_TYPE_B[] = {PROTOCOL_SELECT,0x04,0x03,0x01,0x08,0x01};
static uc8 INIT_TYPE_B[] 	= {PROTOCOL_SELECT,0x02,0x03,0x01};

static uc8 REQB[]			= {SEND_RECEIVE		,0x03,0x05,0x00,0x00};

static uc8 ATTRIB[] 		= {SEND_RECEIVE		,0x09,0x1D,0x00,0x00,0x00,0x00,0x00,0x08,0x01,0x00};

/* Allows you to know if the card is still in the field in PROTOCOL state (only for non-proprietary card) */
static uc8 R_BLOCK[]		= {SEND_RECEIVE,0x01,0xB2};

/**
 * @brief  Reset the ISO14443B data structure
 * @param  void
 * @return void
 */
void ISO14443B_Reset( void )
{
	/* Initializes the data structure used to store results */
	memset(ISO14443B_Card.ATQB , 0x00, ATQB_SIZE);
	memset(ISO14443B_Card.PUPI , 0x00, MAX_PUPI_SIZE);
	ISO14443B_Card.IsDetected  = FALSE;
	memset(ISO14443B_Card.LogMsg, 0x00, 120);
}


/**
 * @brief  Initializes the CR95HF for the IS014443B protocol
 * @param  void
 * @return RESULTOK (if CR95HF is well configured) / ERRORCODE_GENERIC (Communication issue)
 */
static int8_t ISO14443B_Init( uint8_t *pDataRead )
{
//uc8 EchoCmd[2] = {ECHO,0x00};
//
//	if(CR95HF_CheckSendReceive(EchoCmd, pDataRead)!= CR95HF_SUCCESS_CODE)
//		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(INIT_TYPE_B, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;
}

/**
 * @brief  Handles the REQB command
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered ATQB) / ERRORCODE_GENERIC (No ISO14443B in Field)
 */
static int8_t ISO14443B_REQB( uint8_t *pDataRead )
{
//uc8 EchoCmd[2] = {ECHO,0x00};
//
//	if(CR95HF_CheckSendReceive(EchoCmd, pDataRead)!= CR95HF_SUCCESS_CODE)
//		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(REQB, pDataRead)!= CR95HF_SUCCESS_CODE)
		return ERRORCODE_GENERIC;
		
	return RESULTOK;
}

/**
 * @brief  Handles the ATTRIB command
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered) / ERRORCODE_GENERIC (No ISO14443B in Field)
 */
static int8_t ISO14443B_Attrib( uint8_t *pDataRead )
{
	uint8_t DataToSend[MAX_BUFFER_SIZE];

	memcpy(DataToSend    , ATTRIB, MAX_BUFFER_SIZE);
	memcpy(&DataToSend[3], ISO14443B_Card.PUPI, MAX_PUPI_SIZE);

	if(CR95HF_CheckSendReceive(DataToSend, pDataRead)!= CR95HF_SUCCESS_CODE)
		return ERRORCODE_GENERIC;


	return RESULTOK;
}


/**
 * @brief  Checks if a card is in the field
 * @param  void
 * @return RESULTOK (A card is present) / ERRORCODE_GENERIC (No card)
 */
int8_t ISO14443B_IsPresent( void )
{
	uint8_t DataRead[MAX_BUFFER_SIZE];
	uint8_t Tag_error_check;

	/* Initializing buffer */
	memset(DataRead,0xFF,MAX_BUFFER_SIZE);

	/* Init the ISO14443 TypeB communication */
	if(ISO14443B_Init(DataRead) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	

	/* WakeUp attempt */
	if(ISO14443B_REQB(DataRead) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

//	/* exclue le cas 0x80 0x00 .......*/
	if(DataRead[1] == 0x00)
	return ERRORCODE_GENERIC;

	Tag_error_check = DataRead[OFFSET_LENGTH]+ 1;
	if(DataRead[Tag_error_check] & CRC_MASK == CRC_ERROR_CODE)
	return ERRORCODE_GENERIC;

	/* Filling of the data structure */
	ISO14443B_Card.IsDetected = TRUE;
	memcpy(ISO14443B_Card.ATQB, &DataRead[CR95HF_DATA_OFFSET],  ATQB_SIZE);
	memcpy(ISO14443B_Card.PUPI, &ISO14443B_Card.ATQB[1], MAX_PUPI_SIZE);


	/* An ISO14443 card is in the field */	
  return RESULTOK;
}

/**
 * @brief Processing of the Anticolision for ISO14443B cards
 * @param None
 * @return RESULTOK (Anticollision done) / ERRORCODE_GENERIC (Communication issue)
 */
int8_t ISO14443B_Anticollision(void)
{
	uint8_t DataRead[MAX_BUFFER_SIZE], i;
/*
	sprintf(message, "%sISO14443B detected !\n",message);
	sprintf(message, "%s--------------------\n",message);
*/
	sprintf(ISO14443B_Card.LogMsg, "%s- Card ID ----------\n",ISO14443B_Card.LogMsg);
	for(i=0;i<MAX_PUPI_SIZE;i++)
	{	
		sprintf(ISO14443B_Card.LogMsg, "%s%02X",ISO14443B_Card.LogMsg, ISO14443B_Card.PUPI[i]);
	}
	sprintf(ISO14443B_Card.LogMsg, "%s\n", ISO14443B_Card.LogMsg);

	/* ATTRIB attempt */
	if(ISO14443B_Attrib(DataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}

	
	/* Anticollision process done ! */	
  return RESULTOK;
}

/**
 * @brief  Checks if a card is still in the field
 * @param  void
 * @return RESULTOK (A card is still present) / ERRORCODE_GENERIC (No card)
 */
int8_t ISO14443B_CardTest( void )
{
	uint8_t DummyBuffer[MAX_BUFFER_SIZE];

	if(CR95HF_CheckSendReceive(R_BLOCK, DummyBuffer) != CR95HF_SUCCESS_CODE)
		return ERRORCODE_GENERIC;
	
	return RESULTOK;
}


