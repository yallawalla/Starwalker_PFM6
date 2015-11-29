/**
  ******************************************************************************
  * @file    lib_NDEF.c 
  * @author  MMY Application Team
  * @version V0.1
  * @date    30/03/2011
  * @brief   
  * @brief   
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */ 

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LIB_NDEF_H
#define _LIB_NDEF_H

#include "stdint.h"
#include "stm32f10x.h"
#include "miscellaneous.h"
#include "lib_CR95HF.h"


#define NDEF_SUCCESSCODE  						FALSE
#define NDEF_ERRORCODE_DEFAULT					0x50
#define NDEF_ERRORCODE_TYPEFIELD_UNKNOWN		0x51
#define NDEF_ERRORCODE_LENGTHFIELD_RFU			0x52
#define NDEF_ERRORCODE_NOTAGPRESENT				0x53
/**
 *	@brief  TLV block Name
*/
#define NDEF_TLV_NULL 							0x00
#define NDEF_TLV_LOCKCONTROL	 				0x01
#define NDEF_TLV_MEMORYCONTROL 					0x02
#define NDEF_TLV_NDEF_MESSGAGE	 				0x03
#define NDEF_TLV_PROPIRETARY 					0xFD
#define NDEF_TLV_TERMINATOR						0xFE


/**
 *	@brief  
*/
#define NDEF_TYPE_RECORDVALUE					0x54		// 'T' for text record type


/**
 *	@brief  Record header offset
*/
#define NDEF_RECORDHEADER_MB_OFFSET				0x07
#define NDEF_RECORDHEADER_ME_OFFSET	 			0x06
#define NDEF_RECORDHEADER_CF_OFFSET				0x05
#define NDEF_RECORDHEADER_SR_OFFSET	 			0x04
#define NDEF_RECORDHEADER_IL_OFFSET				0x03
#define NDEF_RECORDHEADER_TNF_OFFSET			0x00

#define NDEF_RECORDHEADER_MB_MASK				0x80
#define NDEF_RECORDHEADER_ME_MASK	 			0x40
#define NDEF_RECORDHEADER_CF_MASK				0x20
#define NDEF_RECORDHEADER_SR_MASK	 			0x10
#define NDEF_RECORDHEADER_IL_MASK				0x08
#define NDEF_RECORDHEADER_TNF_MASK				0x07


/**
 *	@brief  Magic number definition 
*/
#define NDEF_MAGIC_NUMBER 					0xE1




void NDEF_SetCCField (uc8 CC0,uc8 CC1,uc8 CC2,uc8 CC3,uint8_t *CCblock);
int8_t 	NDEF_WriteNDEFMessage 	(uc8 *UIDin, uc8 BlockIndex, uc8 NbBytesMessage, uc8 *Message);
int8_t 	NDEF_CreateRecordHeaderByte (uc8 MB, uc8 ME, uc8 CF,uc8 SR,uc8 IL,uc8 TNF);



#endif /* LIB_NDEF_H */
/**
  * @}
  */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
