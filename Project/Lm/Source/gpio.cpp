#include "gpio.h"
#include "isr.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
/**
******************************************************************************
* @file
* @author  Fotona d.d.
* @version
* @date
* @brief	 
*
*/
/** @addtogroup
* @{
*/
/*******************************************************************************
* Function Name	: 12V supply enable, switch scan... 
* Description		:
* Output				:
* Return				: None
*******************************************************************************/
_GPIO::_GPIO() {
			GPIO_InitTypeDef	GPIO_InitStructure;
	
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOB,GPIO_Pin_3 | GPIO_Pin_4);
#if defined(__IOC_V2__)
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
#endif	
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			GPIO_SetBits(GPIOC,GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	
			GPIO_ResetBits(GPIOC,GPIO_Pin_10);	
			timeout=0;
			key = GPIO_ReadInputData(GPIOC) & __MASK;

#if defined(__IOC_V2__)
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif	
}
/*******************************************************************************
* Function Name	: 12V supply enable, keyboard 
* Description		:
* Output				:
* Return				: None
*******************************************************************************/
int   _GPIO::Poll(void) {
			if(key != (GPIO_ReadInputData(GPIOC) & __MASK)) {
				key = GPIO_ReadInputData(GPIOC) & __MASK;
				timeout = __time__ + 20;
			} else if(__time__ > timeout) {
				timeout=INT_MAX;
				return key & __MASK;
			}
			return EOF;
}
/**
* @}
*/
