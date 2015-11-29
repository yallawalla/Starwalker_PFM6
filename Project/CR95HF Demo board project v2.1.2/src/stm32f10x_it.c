/**
  ******************************************************************************
  * @file    stm32f10x_it.c 
  * @author  MMY Application Team
  * @version V1.3
  * @date    20/06/2011
  * @brief   Main Interrupt Service Routines.
  *                      This file provides template for all exceptions handler
  *                      and peripherals interrupt service routine.
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


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "stm32f10x_it.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"

/** @addtogroup User
 * 	@{
 */

/** @addtogroup SRM32_interuption
  * @{
  */

/** @defgroup STM32Interupt_Private_Variables
 *  @{
 */
extern bool 						CR95HF_DataReady;
extern bool 						CR95HF_TimeOut;

/** @defgroup STM32Interupt_Functions
 *  @{
 */

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}


/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : USB_HP_CAN1_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts requests
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_IRQ_HANDLER(void)
{
  CTR_HP();
}

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_IRQ_HANDLER(void)
{
  USB_Istr();
}
 
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/


//#define CR95HF_IRQ_HANDLER							EXTI3_IRQHandlers
void CR95HF_IRQ_HANDLER(void)
{	
	if(EXTI_GetITStatus(EXTI_CR95HF_LINE) != RESET)
	{
		/* Clear IT flag */
		EXTI_ClearITPendingBit(EXTI_CR95HF_LINE);
		/* Disable EXTI Line9 IRQ */
		EXTI->IMR &= ~EXTI_CR95HF_LINE;
		/* CR95HF is ready to send data */
		CR95HF_DataReady = TRUE;
	}
}

//#define TIMER_STANDALONE_IRQ_HANDLER 					TIM2_IRQHandler
void TIMER_STANDALONE_IRQ_HANDLER(void)
{
	/* Clear TIMER update interrupt flag */
	TIM_ClearITPendingBit(TIMER_STANDALONE, TIM_IT_Update);
	//CurrentCard.ACRequest = TRUE;
}
//#define TIMER_TIMEOUT_IRQ_HANDLER			 			TIM3_IRQHandler
void TIMER_TIMEOUT_IRQ_HANDLER(void)
{
	/* Clear TIMER update interrupt flag */
	TIM_ClearITPendingBit(TIMER_TIMEOUT, TIM_IT_Update);
	CR95HF_TimeOut = TRUE;	
	/* Disable the Time out timer */
	TIM_Cmd(TIMER_TIMEOUT, DISABLE);
}

//#define TIMER_DELAY_IRQ_HANDLER							TIM4_IRQHandler
void TIMER_DELAY_IRQ_HANDLER(void)
{
	/* Clear TIMER update interrupt flag */
	TIM_ClearITPendingBit(TIMER_DELAY, TIM_IT_Update);
	decrement_delay();
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
