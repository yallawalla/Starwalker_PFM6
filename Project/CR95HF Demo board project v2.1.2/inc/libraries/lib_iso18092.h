/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso18092.h
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 03/01/2010
* Description        : Send Command Felica
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISO18092_H
#define __ISO18092_H

//#include "bridge.h"
//#include "strfnfca_lib.h"
//#include "error_code_strfnfca.h"
#include "string.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "lib_CR95HF.h"

#define RFU					0
#define TRANSMISSION_DR_212	1
#define TRANSMISSION_DR_424	2
#define RFU2			    3



//#define MAX_BUFFER_SIZE  	255 
/* --------------------------------------------------
 * ISO 14443B  commands -- CMD | Length | data
 * -------------------------------------------------- */

uc8 INIT_TYPE_F[]     = {PROTOCOL_SELECT,							 // Cmd				
										0x03,						 // Length
										0x04,						 // param
										0x11,
										0x10};


uc8 REQC[]				= {SEND_RECEIVE, 						 // Cmd				
										0x05,					 // Length
										0x00,					 // param
										0xFF,
										0xFF,
										0x00,
										0x00
										};


 
uc8 CARD_PRESENT4[]= {SEND_RECEIVE, 						     // Cmd				
										0x05,					 // Length
										0x00,					 // param
										0xFF,
										0xFF,
										0x00,
										0x00
										};
/* ---------------------------------------------------------------------------------
 * --- Local Functions  
 * --------------------------------------------------------------------------------- */
/* ISO14443B functions --- */
int8_t ISO18092_Init							( uint8_t TransmissionDataRate, uint8_t ReceptionDataRate );
int8_t ISO18092_ReqC							( uint8_t *pDataRead );


/* ISO14443B interface --- */
int8_t ISO18092_IsPresent		   ( void );
int8_t ISO18092_Anticollision    ( char *message );
int8_t ISO18092_CardTest		   ( void );
int8_t ISO18092_INITIALISATION(uc8 *pCommand, uint8_t *pResponse);

/* Display utility --- */
void ISO18092_DisplayREQC		 				   ( char *message, uint8_t *pData );


// debug
void STRF_delay(uint16_t t);
#endif /* __ISO14443B_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
