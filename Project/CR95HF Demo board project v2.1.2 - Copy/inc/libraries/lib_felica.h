/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : felica.h
* Author             : SMD Application Team
* Version            : 1.0
* Date               : 16/06/2010
* Description        : Manage the felica communication.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FELICA_H
#define __FELICA_H

#include "lib_CR95HF.h"
#include "usb_type.h"

#define ATQC_SIZE				0x20
#define UID_SIZE_FELICA			8

typedef struct{
	uint8_t ATQC[ATQC_SIZE];
	uint8_t UID	[UID_SIZE_FELICA];
	bool 		IsDetected;
	char		LogMsg[120];
}FELICA_CARD;

extern FELICA_CARD 	FELICA_Card;

/* ---------------------------------------------------------------------------------
 * --- Local Functions  
 * --------------------------------------------------------------------------------- */
void FELICA_Reset		( void );
int8_t FELICA_IsPresent		( void );
int8_t FELICA_CardTest			( void );
int8_t FELICA_Anticollision( void );


#endif /* __FELICA_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
