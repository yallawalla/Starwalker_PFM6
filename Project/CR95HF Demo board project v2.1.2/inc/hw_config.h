/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
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
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/

 #include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define BULK_MAX_PACKET_SIZE  0x00000040

/** 
 * @brief  EXTI Interface pin PA3
 */ 
#define EXTI_CR95HF_PIN        							GPIO_Pin_3
#define EXTI_GPIO_PORT       								GPIOA  
#define EXTI_CR95HF_PIN_SOURCE        			GPIO_PinSource3
#define EXTI_CR95HF_GPIO_CLOCK							RCC_APB2Periph_GPIOA
#define EXTI_CR95HF_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOA
#define EXTI_CR95HF_LINE										EXTI_Line3

/** 
 * @brief  IRQout Interface pin PA2
 */ 
#define IRQOUT_CR95HF_PIN        						GPIO_Pin_2
#define IRQOUT_CR95HF_PIN_SOURCE        		GPIO_PinSource2
#define IRQOUT_CR95HF_GPIO_CLOCK						RCC_APB2Periph_GPIOA
#define IRQOUT_CR95HF_GPIO_PORT_SOURCE			GPIO_PortSourceGPIOA


/** 
 * @brief  Communication Interface pin B1
 */
#define INTERFACE_PIN             					GPIO_Pin_1                  
#define INTERFACE_GPIO_PORT       					GPIOB                   
#define INTERFACE_GPIO_CLK        					RCC_APB2Periph_GPIOB

#ifdef USE_STM3210E_EVAL 
/** 
 * @brief  USB disconnect pin B14
 */

#define USB_DISCONNECT                    GPIOB  
#define USB_DISCONNECT_PIN                GPIO_Pin_14
#define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOB

#else /*DEMO BOARD CR95HF*/
/**
 * @brief  USB disconnect pin B0
 */

#define USB_DISCONNECT                    GPIOB  
#define USB_DISCONNECT_PIN                GPIO_Pin_0
#define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOB

#endif /*USE_STM3210E_EVAL*/

/** 
 * @brief  TIMER definitions 
 */
#define TIMER_STANDALONE							TIM2
#define TIMER_STANDALONE_PERIOD						500
#define TIMER_STANDALONE_PRESCALER					7200
#define TIMER_STANDALONE_CLOCK						RCC_APB1Periph_TIM2

#define TIMER_TIMEOUT								TIM3
#define TIMER_TIMEOUT_PERIOD						300
#define TIMER_TIMEOUT_PRESCALER						7200
#define TIMER_TIMEOUT_CLOCK							RCC_APB1Periph_TIM3

#define TIMER_DELAY									TIM4
#define TIMER_DELAY_PERIOD							1000
#define TIMER_DELAY_PRESCALER						72
#define TIMER_DELAY_CLOCK							RCC_APB1Periph_TIM4

/** 
 * @brief  NVIC definitions 
 */
#define NVIC_PRIORITY_GROUP							NVIC_PriorityGroup_2

#define EXTI_CR95HF_PREEMPTION_PRIORITY				0
#define EXTI_CR95HF_SUB_PRIORITY					0
//#define EXTI_CR95HF_IRQ_CHANNEL						EXTI9_5_IRQn
#define EXTI_CR95HF_IRQ_CHANNEL						EXTI3_IRQn

#define USB_HP_PREEMPTION_PRIORITY					1
//#define USB_HP_SUB_PRIORITY							0
#define USB_HP_SUB_PRIORITY							1
#define USB_HP_IRQ_CHANNEL							USB_HP_CAN1_TX_IRQn

#define USB_LP_PREEMPTION_PRIORITY					1
//#define USB_LP_SUB_PRIORITY							1
#define USB_LP_SUB_PRIORITY							2
#define USB_LP_IRQ_CHANNEL							USB_LP_CAN1_RX0_IRQn

//#define TIMER_TIMEOUT_PREEMPTION_PRIORITY			2
#define TIMER_TIMEOUT_PREEMPTION_PRIORITY			0
#define TIMER_TIMEOUT_SUB_PRIORITY					0
#define TIMER_TIMEOUT_IRQ_CHANNEL					TIM3_IRQn

#define TIMER_STANDALONE_PREEMPTION_PRIORITY		2
#define TIMER_STANDALONE_SUB_PRIORITY				1
#define TIMER_STANDALONE_IRQ_CHANNEL				TIM2_IRQn

#define TIMER_DELAY_PREEMPTION_PRIORITY				2
#define TIMER_DELAY_SUB_PRIORITY					2
#define TIMER_DELAY_IRQ_CHANNEL						TIM4_IRQn


/** 
 * @brief  IRQ functions names 
 */
#define CR95HF_IRQ_HANDLER								EXTI3_IRQHandler
#define USB_HP_IRQ_HANDLER								USB_HP_CAN1_TX_IRQHandler
#define USB_LP_IRQ_HANDLER								USB_LP_CAN1_RX0_IRQHandler
#define TIMER_STANDALONE_IRQ_HANDLER 					TIM2_IRQHandler
#define TIMER_TIMEOUT_IRQ_HANDLER			 			TIM3_IRQHandler
#define TIMER_DELAY_IRQ_HANDLER							TIM4_IRQHandler

/* Exported functions ------------------------------------------------------- */
void Set_USBClock			(void);
void RCC_Config		(void);
void Enter_LowPowerMode		(void);
void Leave_LowPowerMode		(void);
void USB_Cable_Config 		(FunctionalState NewState);
void Set_System(void);
void Interrupts_Config(void);
void IRQOUT_Interrupts_Config(void);
void Led_Configuration(void);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);
void MAL_Config(void);
void USB_Disconnect_Config(void);



void EXTI_Config						( void );
void EXTI_RCC_Config					( void );
void EXTI_GPIO_Config					( void );
void IRQOut_Config						( void );
void SPINSS_Config						( void );
void EXTI_Structure_Config				( void );
void Timer_Config						( void );
void Timer_RCC_Config					( void );
void Timer_Structure_Config				( void );
void Interrupts_Config					( void );
void Interface_Pin_Config				( void );
void Interface_Pin_Config_OutputOpenDrain( void );
void delay_ms							( uint16_t delay );
void delayHighPriority_ms				(uint16_t delay);
void decrement_delay					( void );
void StartTimeOut						( uint16_t delay );
void StopTimeOut						( void );
void stm32_systick_disable				(void);

/* Exported functions ------------------------------------------------------- */


/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
