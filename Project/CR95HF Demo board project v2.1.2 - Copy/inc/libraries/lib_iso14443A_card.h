/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : lib_iso14443A_card.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LIB_ISO14443ACARD_H
#define _LIB_ISO14443ACARD_H

/* succes code */
#define LIB14443ACARD_SUCESSSCODE					RESULTOK

/* error code */
#define LIB14443ACARD_ERRORCODE_GENERIC				0xA0
#define LIB14443ACARD_ERRORCODE_COMMANDUNKNOWN		0xA1
#define LIB14443ACARD_ERRORCODE_CODECOMMAND			0xA2
#define LIB14443ACARD_ERRORCODE_RATSCOMMAND			0xA3
#define LIB14443ACARD_ERRORCODE_SELECTAPPLICATION	0xA4
#define LIB14443ACARD_ERRORCODE_SELECTCCFILE		0xA5
#define LIB14443ACARD_ERRORCODE_SELECTNDEFFILE		0xA6
#define LIB14443ACARD_ERRORCODE_SENDERRORCODE		0xA7


/* Nb of bytes of response */
#define ATS_BUFFER_SIZE								0x10

#define LIB14443ACARD_NFCTYPE4_CLASS				0x00
#define LIB14443ACARD_NFCTYPE4_SELECTCMD			0xA4


// File identifier
#define LIB14443ACARD_NFCTYPE4_CCFILE				0xE103
#define LIB14443ACARD_NFCTYPE4_NDEFFILE				0xE104
#define LIB14443ACARD_NFCTYPE4_NDEFAPPLICATION		0xD276

/* NFC type 4 status code */
#define LIB14443ACARD_NFCTYPE4_STATUSOK				0x9000
#define LIB14443ACARD_NFCTYPE4_APPLICATIONNOTFOUND	0x6A82
/* Nb of bytes of command */

/* Value of Card  */
#define LIB14443ACARD_TL							0x06
#define LIB14443ACARD_T0							0x75
#define LIB14443ACARD_TA1							0x77
#define LIB14443ACARD_TB1							0x81
#define LIB14443ACARD_TC1							0x02

// state of the card emulator 
typedef enum {
 	CARDSTATE_UNKNOWN 	 = 0,
	CARDSTATE_DESACTIVATED,
	CARDSTATE_ACTIVATED,
	CARDSTATE_NO_RFFIELD,
}CARDEMULATOR_STATE;


// select command Identifiers 
typedef enum {
 	CARDPROTOCOL_SELECTCMD_UNKNOWN 	 = 0,
	CARDPROTOCOL_SELECTCMD_SELECTAPPLICATION,
	CARDPROTOCOL_SELECTCMD_SELECTCCFILE,
	CARDPROTOCOL_SELECTCMD_SELECTNDEFFILE,
}CARDEMULATOR_SELECTCMD_ID;



// protocol selected for the card emulator 
typedef enum {
 	CARDPROTOCOL_UNKNOWN 	 = 0,
	CARDPROTOCOL_14443A,
	CARDPROTOCOL_14443B,
	CARDPROTOCOL_18092,
}CARDEMULATOR_PROTOCOL;

// structure of the card emulator
typedef struct {
 	CARDEMULATOR_STATE State;
	CARDEMULATOR_PROTOCOL Protocol;

}CARDEMULATOR;

void CardLibA_InitializeStructure				( void );
void CardLibA_ReceiveCommand					( void );
int8_t CardLibA_DecodeCommand					( uint8_t *pData );
int8_t CardLibA_RATS							( uint8_t *pData );

int8_t CardLibA_SendAnErrorCode					(  uint8_t *pData,uint16_t ErrorCode )	;

int8_t CardLibA_SelectApplication				( uint8_t *pData );
int8_t CardLibA_SelectNDEFfile					( uint8_t *pData );
int8_t CardLibA_SelectCCfile					( uint8_t *pData );

#endif
/**
  * @}
  */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
