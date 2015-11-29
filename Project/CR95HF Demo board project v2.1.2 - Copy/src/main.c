/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : main.c
* Author             : MMY Application Team
* Version            : V2.1.1
* Date               : 12-June-2012
* Description        : USB Composite HID + MSD for STM3210-E EVAL & DEMO BOARD CR95HF
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
#include "drv_LED.h"

#ifdef USE_STM3210E_EVAL
#include "stm3210e_lcd.h"
#endif /*USE_STM3210E_EVAL*/


extern int8_t TagHunting_NFC (void);

ReaderConfigStruct			ReaderConfig;

uint8_t						CR95HFhandled,
									ReaderRecBuf [0xFF],
									TagMemoryBuffer[8192];

extern bool HID_TRANSACTION;	


extern __IO bool CR95HF_DataReady;



/** @addtogroup APPLI
 * 	@{
 */

/** @addtogroup Main
 * 	@{
 */

/** @defgroup Main_Functions
 *  @{
 */



/**
 *	@brief  Main function
 *  @param  None
 *  @return None 
 */
int main(void)
{
	
	Set_System();
	Set_USBClock();
	Led_Configuration();
	Interrupts_Config();
	USB_Init();
	
	#ifdef USE_MSD_DRIVE
	while (bDeviceState != CONFIGURED);
	#endif /*USE_MSD_DRIVE*/
	
	
	/* PWR and BKP clocks selection ------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	/*For SPI Init (Comment for UART)*/
	InitCR95HFConfigStructure (&ReaderConfig);
	
	Timer_Config( );
	
	delay_ms(50);
	
	CR95HF_SerialInterfaceInitilisation ();
	
	CR95HFhandled = CR95HF_PORsequence( );
	
	 if (CR95HFhandled == CR95HF_SUCCESS_CODE)
	 	ReaderConfig.State = CR95HF_STATE_READY;
	
	#ifdef USE_STM3210E_EVAL
		STM3210E_LCD_Init();
		LCD_Clear(White);
	#endif /*USE_STM3210E_EVAL*/
	
	
	LED_On(LED1);
	delay_ms(400);
	LED_Off(LED1);
	
	while (1)
	{
		
		TagHunting_NFC();
		
// 		if ( ReaderConfig.State == CR95HF_STATE_TAGDETECTOR )
// 		{

// 			if (GetIRQOutState () == 0x00)
// 			{
// 					LED_On(LED1);
// 					ReaderConfig.State = CR95HF_STATE_READY;
// 			}
// 			else LED_Off(LED1);

// 		}
		delay_ms(100);

	}
}

#ifdef USE_FULL_ASSERT

/**
 *	@brief  Reports the name of the source file and the source line number
 *          where the assert_param error has occurred.
 *  @param  file: pointer to the source file name
 *  @param  line: assert_param error line source number
 *  @return None 
 */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
 * @}
 */ 
 /**
 * @}
 */ 
 /**
 * @}
 */ 
/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
