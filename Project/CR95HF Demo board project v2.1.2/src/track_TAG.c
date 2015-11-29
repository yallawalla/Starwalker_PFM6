/**
  ******************************************************************************
  * @file   track_TAG.c
  * @author  MMY Application Team
  * @version V1.0
  * @date    25/06/2012
  * @brief   This file provides the set of function to detect a NFC or RFID tag.
  * @brief   The NFC type 1, 2 3 4 and 5 and the SR tags are sought. 
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
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  */

#include "track_TAG.h"

#include "lib_iso15693.h"
#include "lib_M24LRXX.h"

#include "lib_iso14443A.h"
#include "lib_iso14443B.h"
#include "lib_felica.h"
#include "lib_iso14443SR.h"

#include "hw_config.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
#include "drv_LED.h"

#ifdef STM32F10X_HD
#include "stm3210e_lcd.h"


int8_t TagHunting_NFC (void);
static uint8_t User_DisplayUID (uint8_t *ReaderRecBuf, uint8_t protocol_init);
#endif /*STM32F10X_HD*/

bool HID_TRANSACTION = FALSE;

extern uint8_t 	ReaderRecBuf[MAX_BUFFER_SIZE+3]; 	// buffer for SPI ou UART reception

/**  
* @brief  	this function searches if a NFC or RFID tag is in the RF field. 
* @brief  	The method used is this described by the NFC specification
* @param  	none
* @retval 	TRACK_NOTAG 	: No tag in the RF field
* @retval 	TRACK_14443A 	: A ISO/IEC 14443 type A tag is present in the RF field
* @retval 	TRACK_14443B 	: A ISO/IEC 14443 type B tag is present in the RF field
* @retval 	TRACK_14443SR 	: A ISO/IEC 14443 type SR (STM device) tag is present in the RF field
* @retval 	TRACK_18092 	: A ISO/IEC 18092 tag is present in the RF field
* @retval 	FOUND_15693 	: A ISO/IEC 15693 type A tag is present in the RF field
* @retval 	TRACK_NFCTYPE1 	: A NFC type1 tag is present in the RF field
*/
uint8_t	RBuf[]={0x04,0x03,0x30,0x00,0x28};
uint8_t	WBuf[]={0x04,0x07,0xA2,0x0C,0xA5,0xA5,0xA5,0xA5,0x28};
extern uint8_t	TagMemoryBuffer[8192];
bool		FOUND_A=FALSE;

int8_t  TagHunting_NFC (void) {
	
				CR95HF_FieldOn( );
				delay_ms(10);

				if(ISO14443A_IsPresent() == RESULTOK && ISO14443A_Anticollision() == RESULTOK ) {
					if(FOUND_A != TRUE) {
						FOUND_A = TRUE;					
						memset(TagMemoryBuffer,0,sizeof(TagMemoryBuffer));
						SPIUART_SendReceive(RBuf,TagMemoryBuffer);
												
//						memset(RcvBuf,0xff,sizeof(RcvBuf));
//						SPIUART_SendReceive(WBuf,RcvBuf);
//						Write_MSD_File(TagMemoryBuffer, sizeof(TagMemoryBuffer), "0:tag.bin");
//					
//						memset(RcvBuf,0xff,sizeof(RcvBuf));
//						SPIUART_SendReceive(RBuf,RcvBuf);
//						Write_MSD_File(RcvBuf, sizeof(RcvBuf), "0:r2.bin");

//						/*Rebbot the USB connection to force windows to update the FAT table of the MSD*/
						LED_On(LED1);
//						USB_MSD_Reboot();
					}
				} else {
					FOUND_A = FALSE;
					LED_Off(LED1);
				}
			CR95HF_FieldOff	();	
			return TRACK_NOTAG;
	}


#ifdef STM32F10X_HD
/**  
* @brief 	this function displays the contacless tag UID on LCD screen
* @param  	ReaderRecBuf : CR95HF response 	
* @param  	UIDoutindex : index of UID in ReaderRecBuf	
* @retval 	RESULTOK
* @retval 	ERRORCODE_GENERIC
*/
static uint8_t User_DisplayUID (uint8_t *ReaderRecBuf, uint8_t protocol_init)
{
uint8_t 			ASCIIString [26];
int8_t Print_Char_LCD = 0, Buff_ASCII = 0;

uint8_t OFFSET_START = 0;
uint8_t OFFSET_END = 0;
uint8_t UID_LENGTH = 0;

	switch (protocol_init)
	{
	case PROTOCOL_TAG_ISO15693:

	OFFSET_START = 3;
	OFFSET_END =2;
	UID_LENGTH = 16;


	HexToString (	&(ReaderRecBuf[0]),
					UID_LENGTH,
					ASCIIString);

			//invert LSB <=> MSB
			for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH  ; Print_Char_LCD+=2)
            {        
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[Print_Char_LCD+1];
                ASCIIString[Print_Char_LCD+1] = Buff_ASCII;
            }
	
		break;

	 case PROTOCOL_TAG_ISO14443A:

	 UID_LENGTH = ISO14443A_Card.UIDsize*2;
	 OFFSET_END =(21-UID_LENGTH)/2;
	 OFFSET_START = OFFSET_END+1;
	 
	 

	 HexToString (	&(ReaderRecBuf[0]),
					UID_LENGTH,
					ASCIIString);


	 for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH/2  ; Print_Char_LCD++)
            {
                    
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[UID_LENGTH-Print_Char_LCD-1];
                ASCIIString[UID_LENGTH-Print_Char_LCD-1] = Buff_ASCII;
            }

		 break;
	 

	 case PROTOCOL_TAG_ISO14443B:


	 OFFSET_START = 7;
	 OFFSET_END =6;
	 UID_LENGTH = 8;

	 HexToString (	&(ReaderRecBuf[0]),
					UID_LENGTH,
					ASCIIString);


	 for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH/2  ; Print_Char_LCD++)
            {
                    
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[UID_LENGTH-Print_Char_LCD-1];
                ASCIIString[UID_LENGTH-Print_Char_LCD-1] = Buff_ASCII;
            }

		 break;
	 
	 case PROTOCOL_TAG_FELICA:


	 OFFSET_START = 7;
	 OFFSET_END =6;
	 UID_LENGTH = 8;

	 HexToString (	&(ReaderRecBuf[0]),
					UID_LENGTH,
					ASCIIString);


	 for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH/2  ; Print_Char_LCD++)
            {
                    
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[UID_LENGTH-Print_Char_LCD-1];
                ASCIIString[UID_LENGTH-Print_Char_LCD-1] = Buff_ASCII;
            }

		 break;

	 case PROTOCOL_TAG_ISO14443_SR:

	OFFSET_START = 3;
	OFFSET_END =2;
	UID_LENGTH = 16;


	HexToString (	&(ReaderRecBuf[2]),
					UID_LENGTH,
					ASCIIString);

			//invert LSB <=> MSB
			for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH  ; Print_Char_LCD+=2)
            {
                    
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[Print_Char_LCD+1];
                ASCIIString[Print_Char_LCD+1] = Buff_ASCII;
            }
		break;



	case PROTOCOL_TAG_TOPAZ:

	 UID_LENGTH = 8;
	 OFFSET_START = 7;
	 OFFSET_END = 6;

	 
	 

	 HexToString (	&(ReaderRecBuf[4]),
					UID_LENGTH,
					ASCIIString);


	 for (Print_Char_LCD = 0 ; Print_Char_LCD < UID_LENGTH/2  ; Print_Char_LCD++)
            {
                    
				Buff_ASCII = ASCIIString[Print_Char_LCD];
				ASCIIString[Print_Char_LCD] = ASCIIString[UID_LENGTH-Print_Char_LCD-1];
                ASCIIString[UID_LENGTH-Print_Char_LCD-1] = Buff_ASCII;
            }

		 break;

	 }


	 for(Print_Char_LCD =0; Print_Char_LCD<=(UID_LENGTH + OFFSET_START + OFFSET_END) ; Print_Char_LCD++)
		{	
			if( Print_Char_LCD < OFFSET_START || Print_Char_LCD >= UID_LENGTH + OFFSET_START)
			{
			LCD_DisplayChar(Line8, Print_Char_LCD*16,0x20);
			}
			else
			{	
			LCD_DisplayChar(Line8, Print_Char_LCD*16,ASCIIString[Print_Char_LCD-OFFSET_START]);
			}

		}



		return RESULTOK;
	

}
#endif /*STM32F10X_HD*/








