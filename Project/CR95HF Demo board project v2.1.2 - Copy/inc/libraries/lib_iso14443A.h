/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : lib_iso14443A.h
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 03/01/2010
* Description        : Manage the communication between the card and reader
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISO14443A_H
#define __ISO14443A_H

#include "lib_CR95HF.h"
#include "usb_type.h"

/* Anticollison levels (commands) */
#define SEL_CASCADE_LVL_1				0x93
#define SEL_CASCADE_LVL_2				0x95
#define SEL_CASCADE_LVL_3				0x97
#define COMMAND_RATS					0xE0
#define COMMAND_PPS						0xD0

#define COMMAND_IBLOCK02				0x02
#define COMMAND_IBLOCK03				0x03

/* Anticollison levels (enum) */
#define CASCADE_LVL_1					1
#define CASCADE_LVL_2					2
#define CASCADE_LVL_3					3

#define NVM_10							0x10
#define NVM_20							0x20
#define NVM_30							0x30
#define NVM_40							0x40
#define NVM_50							0x50
#define NVM_60							0x60
#define NVM_70							0x70

/* UID Sizes */
#define UIDSIZE_UNDEFINED				-1
#define UID_PART						3
#define UID_SINGLE_SIZE					4
#define	UID_DOUBLE_SIZE					7
#define UID_TRIPLE_SIZE					10


/* Mask used for ATQA */
#define UID_MASK						0xC0
#define AC_BIT_FRAME_MASK				0x1F
#define CID_MASK						0x0F
#define FSDI_MASK						0xF0

/* Size for ISO14443A variables */
#define MAX_NAME_SIZE					50
#define MAX_UID_SIZE			 		10
#define ATQA_SIZE						2

/* SAK FLAG */
#define SAK_FLAG_ATS_SUPPORTED			0x20
#define SAK_FLAG_UID_NOT_COMPLETE		0x04


/*FIELD CONFIG*/
#define Field_ON						0x00
#define Field_OFF						0x01

/* ATQ FLAG */
#define ATQ_FLAG_UID_SINGLE_SIZE		0
#define	ATQ_FLAG_UID_DOUBLE_SIZE		1
#define ATQ_FLAG_UID_TRIPLE_SIZE		2

typedef struct{
	// ATQA answer to request of type A
	uint8_t 	ATQA[ATQA_SIZE];
	uint8_t 	CascadeLevel;
	// UID : unique Idntification
	uint8_t 	UIDsize;
	uint8_t 	UID[MAX_UID_SIZE];
	// SAK : Select acknowledge
	uint8_t 	SAK;
	bool 		ATSSupported;
	bool 		IsDetected;
	char		LogMsg[120];
	uint8_t 	CID,
				FSDI;

}ISO14443A_CARD;

extern ISO14443A_CARD 	ISO14443A_Card;

/* ---------------------------------------------------------------------------------
 * --- Local Functions  
 * --------------------------------------------------------------------------------- */
void ISO14443A_Reset			( void );
int8_t ISO14443A_IsPresent		( void );
int8_t ISO14443A_Anticollision	( void );
int8_t ISO14443A_CardTest			( void );

int8_t TOPAZ_ID( uint8_t *pDataRead);



#endif /* __ISO14443A_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
