/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 03/01/2010
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

typedef enum{
	J_IDDLE = 0,
	J_SEL 	,
	J_DOWN	,
	J_LEFT	,
	J_RIGHT	,
	J_UP
}JOYSTICK_POS;

typedef enum{
	SELECTION_MENU = 0,
	STANDALONE_MENU,
	DEVELOPMENT_MENU,
	EXIT_MENU
}CURRENT_MENU;

typedef enum{
	LEFT_POS = 0,
	RIGHT_POS
}SQUARE_POS;


#define J_SEL_PIN			GPIO_Pin_12
#define J_SEL_PORT			GPIOD
#define J_SEL_CLOCK			RCC_APB2Periph_GPIOD

#define J_DOWN_PIN			GPIO_Pin_14
#define J_DOWN_PORT			GPIOD
#define J_DOWN_CLOCK		RCC_APB2Periph_GPIOD

#define J_LEFT_PIN			GPIO_Pin_1
#define J_LEFT_PORT			GPIOE
#define J_LEFT_CLOCK		RCC_APB2Periph_GPIOE

#define J_RIGHT_PIN			GPIO_Pin_0
#define J_RIGHT_PORT		GPIOE
#define J_RIGHT_CLOCK		RCC_APB2Periph_GPIOE

#define J_UP_PIN			GPIO_Pin_8
#define J_UP_PORT			GPIOD
#define J_UP_CLOCK			RCC_APB2Periph_GPIOD

#define KEYBOUTON_PIN			GPIO_Pin_9
#define KEYBOUTON_PORT			GPIOB
#define KEYBOUTON_CLOCK			RCC_APB2Periph_GPIOB


JOYSTICK_POS JoystickPosition(void);
CURRENT_MENU CheckMenu(void);
void ShowInitialMenu(void);
void InitJoystick(void);
void InitKeyBouton(void);
int8_t KeyBoutonPosition(void);

#endif  /*__MENU_H*/

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

