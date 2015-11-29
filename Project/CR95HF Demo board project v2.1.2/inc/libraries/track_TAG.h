/**
  ******************************************************************************
  * @file    Tack_TAG.h
  * @author  MMY Application Team
  * @version V1.0
  * @date    25/06/2012
  * @brief   This file provides
  *            - set of firmware functions to manage Leds
  *          available on STM3210E-EVAL evaluation board from STMicroelectronics.
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
#ifndef __TRACK_TAG_H
#define __TRACK_TAG_H

#include "stm32f10x.h"
#include "usb_type.h"


typedef enum {
	TRACK_NOTAG = 0,
	TRACK_NFCTYPE1 ,
	TRACK_NFCTYPE2 ,
	TRACK_NFCTYPE3 ,
	TRACK_NFCTYPE4 ,
	TRACK_NFCTYPE5 ,
	TRACK_14443A ,
	TRACK_14443B ,
	TRACK_14443SR ,
	TRACK_15693 ,
	TRACK_18092 ,

}TRACK_TECHNOLOGY;

/*NFC FORUM SPECFICATION*/
bool	CON_POLL_A = TRUE,
		CON_POLL_B = TRUE,
		CON_POLL_F = TRUE,
		CON_POLL_15693 = TRUE,
		CON_POLL_SR = TRUE,
		CON_POLL_TOPAZ = TRUE;

bool	CON_BAIL_OUT_A,
		CON_BAIL_OUT_B;

uint8_t CON_BITR;

uint8_t GRE_POLL_A[],
		GRE_POLL_B[],
		GRE_POLL_F[];


int8_t Field_Config( uint8_t *pDataRead, uint8_t Param );

#endif /*track_TAG.h*/
