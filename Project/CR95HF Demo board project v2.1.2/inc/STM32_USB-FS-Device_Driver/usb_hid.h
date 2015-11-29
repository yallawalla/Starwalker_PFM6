/**
  ******************************************************************************
  * @file    usb_hid.c 
  * @author  MMY Application Team
  * @version V1.3
  * @date    20/06/2011
  * @brief   Handle messages received through USB HID
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
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 


/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __HID_APPLI_H
#define __HID_APPLI_H

/* Includes ----------------------------------------------------------------- */
#include "stm32f10x.h"
#include "drv_spi.h"
#include "drv_CR95HF.h"
				
/* ID definition ------------------------------------------------------------ */
#define HID_SPI_SEND_CR95HFCMD				0x01
#define HID_SPI_SEND_DATA					0x02

#define HID_SEND_HID_RESPONSE				0x07


/* Offset definitions for global buffers */
#define HID_OFFSET							0x00
#define HID_OFFSET_CMDCODE					HID_OFFSET + 1
#define HID_OFFSET_LENGTH					HID_OFFSET + 2
#define HID_OFFSET_CTRLCODE					HID_OFFSET + 3
/* Max buffer size for HID communication */
#define HID_MAX_BUFFER_SIZE					0x40
//#define HID_MAX_BUFFER_SIZE_UPSCALE			0x40

/* Local functions --------------------------------------------------------- */
int8_t HID_Data_Out( void );
void HID_Data_In( void );

#endif /* __HID_APPLI_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
