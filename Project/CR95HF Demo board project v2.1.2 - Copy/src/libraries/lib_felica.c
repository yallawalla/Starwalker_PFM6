/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : Felica.c
* Author             : SMD Application Team
* Version            : 1.0
* Date               : 16/06/2010
* Description        : Manage the FeliCa communication.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "lib_felica.h"
#include "string.h"
#include "stdio.h"
#include "miscellaneous.h"
//#include "main.h"
#include "hw_config.h"

/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup FELICA
 * 	@{
 */

FELICA_CARD 	FELICA_Card;
/* --------------------------------------------------
 * code templates for FELICA protocol
 * command =  Command code | Length | data(Le)
 * -------------------------------------------------- */
//static uc8 INIT_TYPE_C[] = {PROTOCOL_SELECT,0x02,0x04,0x11};
static uc8 INIT_TYPE_C[] = {PROTOCOL_SELECT,0x05,0x04,0x51,0x13,0x01,0x0D};
static uc8 WRITE_AMPLIGAIN[] = {0x09,0x04,0x68,0x01,0x01,0x50};
static uc8 AUTO_FDet[] = {0x09,0x04,0x0A,0x01,0x02,0xA1};
							
static uc8 REQC[] = {SEND_RECEIVE ,0x05,0x00,0xFF,0xFF,0x00,0x03};

/**
 * @brief  Reset the FELICA data structure
 * @param  void
 * @return void
 */
void FELICA_Reset( void )
{
	/* Initializes the data structure used to store results */
	memset(FELICA_Card.ATQC , 0x00, ATQC_SIZE);
	memset(FELICA_Card.UID  , 0x00, UID_SIZE_FELICA);
	FELICA_Card.IsDetected  = FALSE;
	memset(FELICA_Card.LogMsg, 0x00, 120);
}


/**
 * @brief  Initializes the CR95HF for the FELICA protocol
 * @param  void
 * @return TRUE (if CR95HF is well configured) / FALSE (Communication issue)
 */
static int8_t FELICA_Init( uint8_t *pDataRead )
{
//uc8 EchoCmd[2] = {ECHO,0x00};

//	if(CR95HF_CheckSendReceive(EchoCmd, pDataRead)!= CR95HF_SUCCESS_CODE)
//		return ERRORCODE_GENERIC;
	
	if(CR95HF_CheckSendReceive(INIT_TYPE_C, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(WRITE_AMPLIGAIN, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(AUTO_FDet, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;
}

/**
 * @brief  Handles the REQC command
 * @param  *pDataRead	: Pointer on the response
 * @return TRUE (if CR95HF answered ATQB) / FALSE (No ISO14443B in Field)
 */
static int8_t FELICA_REQC( uint8_t *pDataRead )
{
	if(CR95HF_CheckSendReceive(REQC, pDataRead)!= CR95HF_SUCCESS_CODE)
		return ERRORCODE_GENERIC;
		
	return RESULTOK;
}

/**
 * @brief  Checks if a FELICA card is in the field
 * @param  void
 * @return RESULTOK (A card is present) / ERRORCODE_GENERIC (No card)
 */
int8_t FELICA_IsPresent( void )
{
	uint8_t DataRead[MAX_BUFFER_SIZE];

	/* Initializing buffer */
	memset(DataRead,0,MAX_BUFFER_SIZE);

	/* Init the FeliCa communication */
	if(FELICA_Init(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	 delay_ms(20);
	 /*Wakeup attempt*/

	/* REQC attempt */
	if(FELICA_REQC(DataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	/* Filling of the data structure */
	FELICA_Card.IsDetected = TRUE;
	memcpy(FELICA_Card.ATQC, &DataRead[CR95HF_DATA_OFFSET],  ATQC_SIZE);
	memcpy(FELICA_Card.UID , &FELICA_Card.ATQC[1]  , UID_SIZE_FELICA);


//		if(FELICA_Init(DataRead)== ERRORCODE_GENERIC)
//		return ERRORCODE_GENERIC;

	/* An ISO14443 card is in the field */	
  return RESULTOK;
}

/**
 * @brief Processing of the Anticolision for FELICA cards
 * @param *message : contains message on the CR95HF replies
 * @return TRUE (Anticollision done) / FALSE (Communication issue)
 */
int8_t FELICA_Anticollision( void )
{
//uint8_t i;
///*
//	sprintf(message, "%s-- FELICA detected !\n",message);
//	sprintf(message, "%s--------------------\n",message);
//*/
//	sprintf(FELICA_Card.LogMsg, "%s- Card ID ----------\n",FELICA_Card.LogMsg);
//
//	for(i=0;i<UID_SIZE_FELICA;i++)	
//		sprintf(FELICA_Card.LogMsg, "%s%02X",FELICA_Card.LogMsg, FELICA_Card.UID[i]);
//
//	sprintf(FELICA_Card.LogMsg, "%s\n", FELICA_Card.LogMsg);
//
//	/* Anticollision process done ! */	
  return RESULTOK;
}

/**
 * @brief  Checks if a FELICA card is still in the field
 * @param  void
 * @return RESULTOK (A card is still present) / ERRORCODE_GENERIC (No card)
 */
int8_t FELICA_CardTest( void )
{
	uint8_t DummyBuffer[MAX_BUFFER_SIZE];

	if(CR95HF_CheckSendReceive(REQC, DummyBuffer)!= RESULTOK)
		return ERRORCODE_GENERIC;

	return RESULTOK;
}


