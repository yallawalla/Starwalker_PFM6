/**
  ******************************************************************************
  * @file    drv_spi.h 
  * @author  MMY Application Team
  * @version V1.1
  * @date    15/03/2011
  * @brief   This file provides   set of firmaware functions to manages SPI communications
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __SPI_H
#define __SPI_H

/* Includes ----------------------------------------------------------------- */
#include "stm32f10x.h"

#define SPI_RESPONSEBUFFER_SIZE		255

/** @addtogroup Drivers
 * 	@{
 */

/** @addtogroup SPI
 * 	@{
 */

/** @defgroup SPI_Exported_Functions
 *  @{
 */

void SPI_SendByte(SPI_TypeDef* SPIx, uint8_t data);
void SPI_SendWord(SPI_TypeDef* SPIx, uint16_t data);

uint8_t SPI_SendReceiveByte		(SPI_TypeDef* SPIx, uint8_t data); 
void 		SPI_SendReceiveBuffer	(SPI_TypeDef* SPIx, uc8 *pCommand, uint8_t length, uint8_t *pResponse);

/**
 * @}
 */ 

/**
 * @}
 */ 

/**
 * @}
 */ 


#endif /* __SPI_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
