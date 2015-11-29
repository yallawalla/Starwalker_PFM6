/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso14443A.c
* Author             : SMD Application Team
* Version            : 1.0
* Date               : 18/05/2010
* Description        : Manage the iso14443A communication.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#include "lib_iso14443A.h"
#include "string.h"
#include "stdio.h"
#include "hw_config.h" 
#include "miscellaneous.h"
//#include "main.h"

ISO14443A_CARD 	ISO14443A_Card;
/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup ISO14443A
 * 	@{
 */

/* --------------------------------------------------
 * code templates for ISO14443A protocol
 * command =  Command code | Length | data(Le)
 * -------------------------------------------------- */
//static uc8 INIT_TYPE_A[] 			= {PROTOCOL_SELECT, 0x02,	0x02,	0x00};
static uc8 INIT_TYPE_A[] 			= {PROTOCOL_SELECT, 0x04, 0x02, 0x00, 0x01, 0x80};						
//uc8 INIT_TYPE_A[] 					= {PROTOCOL_SELECT, 0x04,	0x02,	0x00, 0x08, 0x05};


static uc8 CONFIG[] = {WRITE_REGISTER, 0x04, 0x3A, 0x00, 0x58, 0x04};
static uc8 DEMOD_GAIN[] = {WRITE_REGISTER, 0x04, 0x68, 0x01, 0x01, 0xD7};

static uc8 TOPAZ[] = {SEND_RECEIVE, 0x08, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8};
	
static uc8 REQA[]					= {SEND_RECEIVE, 0x02, 0x26, 0x07};

static uc8 AC_CASCADE_LVL1[]		= {SEND_RECEIVE, 0x03, 0x93, 0x20, 0x08};

static uc8 AC_CASCADE_LVL2[]		= {SEND_RECEIVE, 0x03, 0x95, 0x20, 0x08};

static uc8 RATS[]					= {SEND_RECEIVE, 0x03, 0xE0, 0x00, 0x28};

static uc8 PPS[]					= {SEND_RECEIVE, 0x04, 0xD0, 0x11, 0x00, 0x28};

static uc8 R_BLOCK[]				= {SEND_RECEIVE, 0x02, 0xB2, 0x28};

/**
 * @brief  Reset the ISO14443A data structure
 * @param  void
 * @return void
 */
void ISO14443A_Reset( void )
{
	/* Initializes the data structure used to store results */
	memset(ISO14443A_Card.ATQA, 0x00, ATQA_SIZE);
	memset(ISO14443A_Card.UID , 0x00, MAX_UID_SIZE);
	ISO14443A_Card.CascadeLevel 	= 0;
	ISO14443A_Card.UIDsize 			= 0;		
	ISO14443A_Card.ATSSupported 	= FALSE;
	ISO14443A_Card.IsDetected   	= FALSE;
	memset(ISO14443A_Card.LogMsg, 0x00, 120);
}

/**
 * @brief  Initializes the CR95HF for the IS014443A protocol
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF is well configured) / ERRORCODE_GENERIC (Communication issue)
 */
static int8_t ISO14443A_Init( uint8_t *pDataRead )
{
//uc8 EchoCmd[2] = {ECHO,0x00};
//
//	if(CR95HF_CheckSendReceive(EchoCmd, pDataRead)== ERRORCODE_GENERIC)
//		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(INIT_TYPE_A, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	if(CR95HF_CheckSendReceive(CONFIG, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

   	if(CR95HF_CheckSendReceive(DEMOD_GAIN, pDataRead)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	
	return RESULTOK;
}





/**
 * @brief  Initializes the CR95HF for the IS014443A protocol
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF is well configured) / ERRORCODE_GENERIC (Communication issue)
 */
int8_t TOPAZ_ID( uint8_t *pDataRead)
{
uint8_t Tag_error_check;

		if(CR95HF_CheckSendReceive(TOPAZ, pDataRead)== ERRORCODE_GENERIC)
			return ERRORCODE_GENERIC;

		Tag_error_check = pDataRead[OFFSET_LENGTH]-1;
		if(pDataRead[Tag_error_check] & CRC_MASK_TYPE_A	 == CRC_ERROR_CODE_TYPE_A)
			return ERRORCODE_GENERIC;

		if(pDataRead[0] == 0x80 && pDataRead[1] == 0x04)
			return ERRORCODE_GENERIC;
	   	
	return RESULTOK;
}





/**
 * @brief  Handles the REQA command
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered ATQB) / ERRORCODE_GENERIC (No ISO14443A in Field)
 */
static int8_t ISO14443A_REQA( uint8_t *pDataRead )
{
	if(CR95HF_CheckSendReceive(REQA, pDataRead)!= RESULTOK)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}


/**
 * @brief  Handles the ATTRIB command
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered) / ERRORCODE_GENERIC (No ISO14443A in Field)
 */
static int8_t ISO14443A_ACLevel1( uint8_t *pDataRead )
{
uint8_t DataToSend[MAX_BUFFER_SIZE];
uint8_t Tag_error_check;

	if(CR95HF_CheckSendReceive(AC_CASCADE_LVL1, pDataRead)!= RESULTOK)
	{
		return ERRORCODE_GENERIC;
	}

	Tag_error_check = pDataRead[OFFSET_LENGTH] -1;
	if(pDataRead[Tag_error_check] & CRC_MASK == CRC_ERROR_CODE)
	return ERRORCODE_GENERIC;
	
//	if(pDataRead[2] == 0x88)
//	ISO14443A_Card.CascadeLevel =0x02;

	if(pDataRead[0] == 0x80 && pDataRead[1] == 0x03)
	return ERRORCODE_GENERIC;
	
	if(ISO14443A_Card.UIDsize == UID_SINGLE_SIZE)
	{
		/* Copy UID into the data structure */
		memcpy( ISO14443A_Card.UID, &pDataRead[CR95HF_DATA_OFFSET], UID_SINGLE_SIZE );
	}
	else
	{
		/* Copy an UID part into the data structure */
		memcpy(ISO14443A_Card.UID, &pDataRead[CR95HF_DATA_OFFSET + 1], UID_PART);
	}
	
	/* Preparing the buffer who contains the SELECT command */
	DataToSend[CR95HF_COMMAND_OFFSET ] = SEND_RECEIVE;
	DataToSend[CR95HF_LENGTH_OFFSET  ] = pDataRead[CR95HF_LENGTH_OFFSET];
	DataToSend[CR95HF_DATA_OFFSET    ]	= SEL_CASCADE_LVL_1;
	DataToSend[CR95HF_DATA_OFFSET + 1] = NVM_70;
	/* Inserts the previous reply in the next command */
	memcpy(&DataToSend[CR95HF_DATA_OFFSET + 2], &pDataRead[CR95HF_DATA_OFFSET], DataToSend[CR95HF_LENGTH_OFFSET] - 2);

	/* Transmits the SELECT command to CR95HF */
	if(CR95HF_CheckSendReceive(DataToSend, pDataRead)!= CR95HF_SUCCESS_CODE)
	{
		return ERRORCODE_GENERIC;
	}

	Tag_error_check = pDataRead[OFFSET_LENGTH] -1;
	if(pDataRead[Tag_error_check] & CRC_MASK_TYPE_A == CRC_ERROR_CODE_TYPE_A)
	return ERRORCODE_GENERIC;

	/* Recovering SAK byte */
	ISO14443A_Card.SAK = pDataRead[CR95HF_DATA_OFFSET];

	/* Anticollision cascade 1 is done ! */
	return RESULTOK;
}

/**
 * @brief  Handles the level 2 of the anticollision loop
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered) / ERRORCODE_GENERIC (No ISO14443B in Field)
 */
static int8_t ISO14443A_ACLevel2( uint8_t *pDataRead )
{
uint8_t DataToSend[MAX_BUFFER_SIZE];
uint8_t Tag_error_check;

	if(CR95HF_CheckSendReceive(AC_CASCADE_LVL2, pDataRead)!= CR95HF_SUCCESS_CODE)
	{
		return ERRORCODE_GENERIC;
	}

	if(pDataRead[0] == 0x80 && pDataRead[1] == 0x03)
	return ERRORCODE_GENERIC;


	if(ISO14443A_Card.UIDsize == UID_DOUBLE_SIZE)
	{
		/* Copy UID into the data structure */
		memcpy(&ISO14443A_Card.UID[UID_PART], &pDataRead[CR95HF_DATA_OFFSET], UID_SINGLE_SIZE);
	}
	else
	{
		/* Copy an UID part into the data structure */
		memcpy(&ISO14443A_Card.UID[UID_PART], &pDataRead[CR95HF_DATA_OFFSET + 1], UID_PART);
	}

	/* Preparing the buffer who contains the SELECT command */
	DataToSend[CR95HF_COMMAND_OFFSET ] = SEND_RECEIVE;
	DataToSend[CR95HF_LENGTH_OFFSET  ] = pDataRead[CR95HF_LENGTH_OFFSET];
	DataToSend[CR95HF_DATA_OFFSET    ]	= SEL_CASCADE_LVL_2;
	DataToSend[CR95HF_DATA_OFFSET + 1] = NVM_70;
	/* Inserts the previous reply in the next command */
	memcpy(&DataToSend[CR95HF_DATA_OFFSET + 2], &pDataRead[CR95HF_DATA_OFFSET], DataToSend[CR95HF_LENGTH_OFFSET] - 2);

	/* Transmits the SELECT command to CR95HF */
	if(CR95HF_CheckSendReceive(DataToSend, pDataRead)!= CR95HF_SUCCESS_CODE)
	{
		return ERRORCODE_GENERIC;
	}

	Tag_error_check = pDataRead[OFFSET_LENGTH] -1;
	if(pDataRead[Tag_error_check] & CRC_MASK_TYPE_A == CRC_ERROR_CODE_TYPE_A)
	return ERRORCODE_GENERIC;

	/* Recovering SAK byte */
	ISO14443A_Card.SAK = pDataRead[CR95HF_DATA_OFFSET];

	/* Anticollision cascade 2 is done ! */
	return RESULTOK;
}

/**
 * @brief  Handles the WAKE_UP command
 * @param  *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered) / ERRORCODE_GENERIC (Communication issue)
 */
static int8_t ISO14443A_RATS( uint8_t *pDataRead )
{
	if(CR95HF_CheckSendReceive(RATS, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}

/**
 * @brief Handles the WAKE_UP command
 * @param *pDataRead	: Pointer to the response
 * @return RESULTOK (if CR95HF answered) / ERRORCODE_GENERIC (Communication issue)
 */
static int8_t ISO14443A_PPS( uint8_t *pDataRead )
{
	if(CR95HF_CheckSendReceive(PPS, pDataRead)== ERRORCODE_GENERIC)
	{
		return ERRORCODE_GENERIC;
	}
	
	return RESULTOK;
}

/**
 * @brief  Checks if a ISO14443A card is in the field
 * @param  void
 * @return RESULTOK (A card is present) / ERRORCODE_GENERIC (No card)
 */
int8_t ISO14443A_IsPresent( void )
{
uint8_t DataRead[MAX_BUFFER_SIZE], flag;

	/* Initializing buffer */
	memset(DataRead, 0x00, MAX_BUFFER_SIZE);

	/* Init the ISO14443 TypeA communication */
	if(ISO14443A_Init(DataRead) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	/* WakeUp attempt */
	if(ISO14443A_REQA(DataRead) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;
	
	if(DataRead[0] == 0x80 && DataRead[1] == 0x03)
		return ERRORCODE_GENERIC;


	ISO14443A_Card.IsDetected = TRUE;
	memcpy(ISO14443A_Card.ATQA, &DataRead[CR95HF_DATA_OFFSET], ATQA_SIZE);

	// acc to FSP ISO 11443-3 the b7&b8 bits of ATQA tag answer is UID size bit frame
	flag = (ISO14443A_Card.ATQA[0] & UID_MASK)>>6;

	/* Recovering the UID size */
	if(flag == ATQ_FLAG_UID_SINGLE_SIZE)
	{
		ISO14443A_Card.UIDsize 			= UID_SINGLE_SIZE;
		ISO14443A_Card.CascadeLevel 	= CASCADE_LVL_1;
	}
	else if(flag == ATQ_FLAG_UID_DOUBLE_SIZE)
	{
		ISO14443A_Card.UIDsize 			= UID_DOUBLE_SIZE;
		ISO14443A_Card.CascadeLevel 	= CASCADE_LVL_2;
	}
	else if(flag == ATQ_FLAG_UID_TRIPLE_SIZE)
	{
		ISO14443A_Card.UIDsize 			= UID_TRIPLE_SIZE;
		ISO14443A_Card.CascadeLevel 	= CASCADE_LVL_3;
	}
//	else  ISO14443A_Card.UIDsize 		= UIDSIZE_UNDEFINED

	/* An ISO14443A card is in the field */	
  return RESULTOK;
}


/**
 * @brief  Checks if a card is in the field
 * @param  None
 * @return RESULTOK (Anticollision done) / ERRORCODE_GENERIC (Communication issue)
 */
int8_t ISO14443A_Anticollision( void )
{
uint8_t DataRead[MAX_BUFFER_SIZE];//i;

	//sprintf(ISO14443A_Card.LogMsg, "%sATQA = %02X%02X \n",ISO14443A_Card.LogMsg, ISO14443A_Card.ATQA[0], ISO14443A_Card.ATQA[1]);
	memset (ISO14443A_Card.UID,0x00,MAX_UID_SIZE);
	/* Checks if an error occured and execute the Anti-collision level 1*/
	if(ISO14443A_ACLevel1(DataRead)== ERRORCODE_GENERIC)
	{
		//sprintf(ISO14443A_Card.LogMsg, "%s..AC level1 err \n", ISO14443A_Card.LogMsg);
		return ERRORCODE_GENERIC;
	}


	/* UID Complete ? */
	if(ISO14443A_Card.SAK & SAK_FLAG_UID_NOT_COMPLETE)
	{
		/* Checks if an error occured and execute the Anti-collision level 2*/
		if(ISO14443A_ACLevel2(DataRead) == ERRORCODE_GENERIC)
		{
			//sprintf(ISO14443A_Card.LogMsg, "%s..AC level2 err \n", ISO14443A_Card.LogMsg);
			return ERRORCODE_GENERIC;
		}
	}

//	/* Displaying UID */
//	sprintf(ISO14443A_Card.LogMsg, "%s- Card ID ----------\n",ISO14443A_Card.LogMsg);
//	for(i=0;i<ISO14443A_Card.UIDsize;i++)
//	{		
//		sprintf(ISO14443A_Card.LogMsg, "%s%02X",ISO14443A_Card.LogMsg, ISO14443A_Card.UID[i]);
//	}
//	sprintf(ISO14443A_Card.LogMsg, "%s\n", ISO14443A_Card.LogMsg);

	/* Checks if the ATS command is supported by the card */
	if(ISO14443A_Card.SAK & SAK_FLAG_ATS_SUPPORTED)
	{
		ISO14443A_Card.ATSSupported = TRUE;
		//sprintf(ISO14443A_Card.LogMsg, "%sISO14443-4 compliant\n", ISO14443A_Card.LogMsg);
		if(ISO14443A_RATS(DataRead) == ERRORCODE_GENERIC)
		{
			//sprintf(ISO14443A_Card.LogMsg, "%sRATS failed\n", ISO14443A_Card.LogMsg);	
			return ERRORCODE_GENERIC;
		}
		if(ISO14443A_PPS(DataRead) == ERRORCODE_GENERIC)
		{
			//sprintf(ISO14443A_Card.LogMsg, "%sPPS failed\n", ISO14443A_Card.LogMsg);	
			return ERRORCODE_GENERIC;
		}
	}
//	else
//	{
//		sprintf(ISO14443A_Card.LogMsg, "%sProprietary Cmd\n", ISO14443A_Card.LogMsg);
//	}
			
	/* Anticollision process done ! */	
  return RESULTOK;

}

/**
 * @brief  Checks if a ISO14443A card is still in the field
 * @param  None
 * @return RESULTOK (A card is still present) / ERRORCODE_GENERIC (No card)
 */
int8_t ISO14443A_CardTest( void )
{
	uint8_t DummyBuffer[MAX_BUFFER_SIZE];
	/* If the ATS command is supported, An R-Block is send to check if the card is still there ...*/
	if(ISO14443A_Card.ATSSupported)
	{
		if(CR95HF_CheckSendReceive(R_BLOCK, DummyBuffer) != RESULTOK)
		{	
			/* The card is no longer in the field */
			return ERRORCODE_GENERIC;
		}
	}
	/* ... else two REQA are sent to check the card status */
	else
	{
		/* The first send is a dummy command */
		CR95HF_CheckSendReceive(REQA, DummyBuffer);

		/* The card should answer to that one */
		if(CR95HF_CheckSendReceive(REQA, DummyBuffer) != RESULTOK)
		{	
			/* The card is no longer in the field */
			return ERRORCODE_GENERIC;
		}
	}
	/* The card is still there ! */
	return RESULTOK;
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
