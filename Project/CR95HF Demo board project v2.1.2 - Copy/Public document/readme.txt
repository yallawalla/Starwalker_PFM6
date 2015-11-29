/**
  ******************************************************************************
  * @file    CR95HF demobaord firmware
  * @author  MMY Application Team
  * @version V2.1.1
  * @date    07/12/2011
  * @brief   this firmware translates the USB command provided by the PC to 
  * @brief   serial command (SPI or UART)
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
*/


this firmware has been developped on CR95HF_DEMO_BOARDAboard by using uvision 4.0 (Keil).
1.1.0 : first release

1.2.0 : 
	- Add timeout during USB transaction
	- add IRQ_IN pulse command
1.3.0 :
	- add SPI NSS pulse command
	- add polling reading command
1.4.0 :
	- UART caud rate limitation 
	- Add CR95HF_ReadTagSectorMemory command (ISO 15693)
	- Add CR95HF_DownloadSTM32Memory command
	- decrease the time between two HID frame (1 ms vs 16ms)
	- change MCU target
	
1.4.1 :	
	- add the ISO15693 anticollision function

2.1.1 :
	- add the compatibility with the STM3210E_EVAL baord
	- add the tag detecting & tag calibration functions
	- add the USB mass storage feature
	- add the tag hunting function

known issue :
	- change of UART baud rate









