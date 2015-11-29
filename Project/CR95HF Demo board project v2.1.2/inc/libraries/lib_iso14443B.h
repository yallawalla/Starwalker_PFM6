/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso14443B.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISO14443B_H
#define __ISO14443B_H

#include "lib_CR95HF.h"
#include "usb_type.h"

#define ATQB_SIZE					13
#define MAX_PUPI_SIZE			4

typedef struct{
	// ATQB answer to request of type B
	uint8_t 	ATQB[ATQB_SIZE];
	// PUPI : pseudo Unique PICC identifier
	// PICC : proximity card
	uint8_t 	PUPI[MAX_PUPI_SIZE];
	bool 		IsDetected;
	char		LogMsg[120];
}ISO14443B_CARD;

extern ISO14443B_CARD 	ISO14443B_Card;

/* ---------------------------------------------------------------------------------
 * --- Local Functions  
 * --------------------------------------------------------------------------------- */
void ISO14443B_Reset				( void );
int8_t ISO14443B_IsPresent		( void );
int8_t ISO14443B_CardTest			( void );
int8_t ISO14443B_Anticollision( void );


#endif /* __ISO14443B_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
