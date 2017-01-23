/**
  ******************************************************************************
  * @file    usbd_storage_msd.c
  * @author  MCD application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the disk operations functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_mem.h"
#include "usb_conf.h"

#include <stdio.h>
#include <string.h>
#include <integer.h>

#include "pfm.h"
#include "diskio.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup STORAGE 
  * @brief media storage application module
  * @{
  */ 

/** @defgroup STORAGE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Defines
  * @{
  */ 
int8_t	STORAGE_Init (uint8_t);
int8_t	STORAGE_GetCapacity (uint8_t, uint32_t *, uint32_t *);
int8_t	STORAGE_IsReady (uint8_t);
int8_t	STORAGE_IsWriteProtected (uint8_t);
int8_t	STORAGE_Read (uint8_t, uint8_t *, uint32_t, uint16_t);
int8_t	STORAGE_Write (uint8_t, uint8_t *, uint32_t, uint16_t);
int8_t	STORAGE_GetMaxLun (void);

#define STORAGE_LUN_NBR                  2
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Variables
  * @{
  */ 
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', /* Product      : 16 Bytes */
  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                     /* Version      : 4 Bytes */
}; 

/**
  * @}
  */ 


/** @defgroup STORAGE_Private_FunctionPrototypes
  * @{
  */ 

USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;
/**
  * @}
  */ 


/** @defgroup STORAGE_Private_Functions
  * @{
  */ 

/*-----------------------------------------------------------------------*/
int						FLASH_Program(uint32_t Address, uint32_t Data) {
int						m;
							FLASH_Unlock();
							FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);	
							do {
								Watchdog();	
								m=FLASH_ProgramWord(Address,Data);
							} while (m==FLASH_BUSY);
							return(m);
}
/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */
//_____________________________________________________________________________________________
int8_t STORAGE_Init (uint8_t lun)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	return (RES_OK);
}
//_____________________________________________________________________________________________
/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
	*block_size=SECTOR_SIZE;
	*block_num=SECTOR_COUNT;
	return (RES_OK);
}
//_____________________________________________________________________________________________
/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{
	return (RES_OK);
}
//_____________________________________________________________________________________________
/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
	return (RES_OK);
}
//_____________________________________________________________________________________________
/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
//_____________________________________________________________________________________________
int8_t STORAGE_Read(uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{
int i,*p,*q=NULL;
	for(p=(int *)FATFS_ADDRESS; (int)p < FATFS_ADDRESS + PAGE_SIZE*PAGE_COUNT &&  p[SECTOR_SIZE/4]!=-1; p=&p[SECTOR_SIZE/4+1]) {
		if(p[SECTOR_SIZE/4] == blk_addr)
			q=p;
	}
	if((int)p >= FATFS_ADDRESS + PAGE_SIZE*PAGE_COUNT)
		return RES_ERROR;
	if(q)
		p=q;
	q=(int *)buf;
	for(i=0;i<SECTOR_SIZE/4; ++i)
		*q++=~(*p++);	
	if(--blk_len)
		return STORAGE_Read (lun, (uint8_t *)q, ++blk_addr, blk_len);
	return RES_OK; 
}
//_____________________________________________________________________________________________
/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
int i,*p,*q=NULL;
	for(p=(int *)FATFS_ADDRESS; (int)p < FATFS_ADDRESS + PAGE_SIZE*PAGE_COUNT &&  p[SECTOR_SIZE/4]!=-1; p=&p[SECTOR_SIZE/4+1])
		if(p[SECTOR_SIZE/4] == blk_addr)
			q=p;

	if((int)p >= FATFS_ADDRESS + PAGE_SIZE*PAGE_COUNT)
		return RES_ERROR;

	q=(int *)buf;
	for(i=0; i<SECTOR_SIZE/4; ++i)	
		if(*q++)
			break;

	if(i<SECTOR_SIZE/4) {												// all zeroes ???
		q=(int *)buf;
		for(i=0; i<SECTOR_SIZE/4; ++i,++p,++q)
			FLASH_Program((int)p,~(*q));
		FLASH_Program((int)p,blk_addr);
	}
	
	if(--blk_len)
		return STORAGE_Write (lun, (uint8_t *)q, ++blk_addr, blk_len);
	return RES_OK; 
}	  
//_____________________________________________________________________________________________
/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
}
/**
  * @}
  */ 
/*-----------------------------------------------------------------------*/
DWORD get_fattime (void) {
			return __time__;
}
/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
