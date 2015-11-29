/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso14443SR.h
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 03/01/2010
* Description        : Send commands SR
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISO14443SR_H
#define __ISO14443SR_H

//#include "bridge.h"
//#include "strfnfca_lib.h"
//#include "error_code_strfnfca.h"
#include "string.h"
#include "stdio.h"
#include "lib_CR95HF.h"

#define TRANSMISSION_DR_106	0
#define TRANSMISSION_DR_212	1
#define TRANSMISSION_DR_424	2
#define TRANSMISSION_DR_848	3

#define RECEPTION_DR_106	0
#define RECEPTION_DR_212	1
#define RECEPTION_DR_424	2
#define RECEPTION_DR_848	3

//#define MAX_BUFFER_SIZE  	255 

#define ISO144443SR_NBBYTE_UID 		8

// IC value									
#define ISO144443SR_ICREF_SRI2K 		0x0F
#define ISO144443SR_ICREF_SRI4K 		0x07
#define ISO144443SR_ICREF_SRIX4K 		0x03
#define ISO144443SR_ICREF_SRI512 		0x06
#define ISO144443SR_ICREF_SRT512 		0x0C


	

									

  
/* ---------------------------------------------------------------------------------
 * --- Local Functions  
 * --------------------------------------------------------------------------------- */
/* ISO14443SRfunctions --- */
int8_t ISO14443SR_Init			    ( uint8_t TransmissionDataRate, uint8_t ReceptionDataRate );
int8_t ISO14443SR_Initiate			( uint8_t *pDataRead );
int8_t ISO14443SR_Select_Chip_ID		( uint8_t *pDataRead, uint8_t *Chip_ID  );
int8_t ISO14443SR_GetUID			    ( uint8_t *pDataRead );
int8_t ISO14443SR_ReadBlock			( uint8_t *pDataRead );
int8_t ISO14443SR_WriteBlock			( uint8_t *pDataRead );

/* ISO14443SR interface --- */
int8_t ISO14443SR_IsPresent		   ( void );
int8_t ISO14443SR_Anticollision      ( char *message );
int8_t ISO14443SR_CardTest		   ( void );

int8_t ISO14443_Ex14443(uc8 *pCommand, uint8_t *pResponse);
int8_t ISO14443_Ex14443_3(uc8 *pCommand, uint8_t *pResponse);


/* Display utility --- */
void ISO14443SR_DisplayINITIATE		      ( char *message, uint8_t *pData );
void ISO14443SR_DisplaySELECT_CHIP_ID	  ( char *message, uint8_t *pData );
void ISO14443SR_DisplayGET_UID       	  ( char *message, uint8_t *pData );
void ISO14443SR_DisplayREAD_BLOCK      	  ( char *message, uint8_t *pData );
void ISO14443SR_DisplayWRITE_BLOCK        ( char *message, uint8_t *pData );


/* debug */
void STRF_delay1(uint16_t t);
#endif /* __ISO14443SR_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
