/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MMY Application Team
* Version            : V2.0.0
* Date               : 12-June-2012
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "hw_config.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_lib.h"
#include "lib_CR95HF.h"
#include "string.h"
#include "stdio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "drv_LED.h"
#ifdef USE_MSD_DRIVE
#include "mass_mal.h"
#endif /*USE_MSD_DRIVE*/


/** @addtogroup APPLI
 * 	@{
 */

/** @addtogroup HW_Config
 * 	@{
 */
ErrorStatus HSEStartUpStatus;
ErrorStatus HSEStartUpStatus;
static __IO uint16_t					counter_delay_ms;


/** @defgroup HWconfig_Extern_Variables
 *  @{
 */
extern CR95HF_INTERFACE				CR95HF_Interface;
extern bool 						CR95HF_TimeOut;
extern ReaderConfigStruct			ReaderConfig;

/**
  * @}
  */

static void IRQOut_GPIO_Config( void );

#ifdef USE_MSD_DRIVE
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
#endif /*USE_MSD_DRIVE*/

/* Private functions ---------------------------------------------------------*/



/** @defgroup HW_Config_Functions
 *  @{
 */

/**
 *	@brief  Configures Main system clocks & power
 *  @param  None
 *  @return None 
 */
void Set_System(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */ 
#if defined(USB_USE_EXTERNAL_PULLUP)
//  GPIO_InitTypeDef  GPIO_InitStructure;
#endif /* USB_USE_EXTERNAL_PULLUP */


  /* Enable and Disconnect Line GPIO clock */
  USB_Disconnect_Config();


#if defined(USB_USE_EXTERNAL_PULLUP)
  /* Enable the USB disconnect GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);

  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);  
#endif /* USB_USE_EXTERNAL_PULLUP */  

#ifdef USE_MSD_DRIVE
  /* MAL configuration */
  MAL_Config();
#endif /*USE_MSD_DRIVE*/
}


/**
 *	@brief  Configures USB Clock input (48MHz)
 *  @param  None
 *  @return None 
 */
void Set_USBClock(void)
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}


/**
 *	@brief  Power-off system clocks and power while entering suspend mode
 *  @param  None
 *  @return None 
 */
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}


/**
 *	@brief  Restores system clocks and power while exiting suspend mode
 *  @param  None
 *  @return None 
 */
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }

}


/**
 *	@brief  Configures the USB interrupts
 *  @param  None
 *  @return None 
 */
void Interrupts_Config (void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


  if (ReaderConfig.SpiMode == SPI_INTERRUPT)
	{
		/* Enable and set CR95HF IRQ to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel 						= EXTI_CR95HF_IRQ_CHANNEL; 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= EXTI_CR95HF_PREEMPTION_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= EXTI_CR95HF_SUB_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
		NVIC_Init(&NVIC_InitStructure); 						
	}

	/* Enable and set TIMER IRQ used for timeout */
	NVIC_InitStructure.NVIC_IRQChannel 							= TIMER_TIMEOUT_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= TIMER_TIMEOUT_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 		= TIMER_TIMEOUT_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd 						= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable and set TIMER IRQ used for delays */ 
	NVIC_InitStructure.NVIC_IRQChannel 					 		= TIMER_DELAY_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= TIMER_DELAY_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 		= TIMER_DELAY_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd 						= ENABLE;
	NVIC_Init(&NVIC_InitStructure);						 	


  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}


/**
 *	@brief  Configures the USB interrupts
 *  @param  None
 *  @return None 
 */
 void IRQOUT_Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	
	/* Enable and set CR95HF IRQ to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel 						= EXTI_CR95HF_IRQ_CHANNEL; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= EXTI_CR95HF_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= EXTI_CR95HF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure); 						

}

/**
 *	@brief  configure the LEDs.
 *  @param  None
 *  @return None 
 */
void Led_Configuration(void)
{
	LED_Config(LED1);	  
}


/**
 *	@brief  Software Connection/Disconnection of USB Cable.
 *  @param  None
 *  @return None 
 */
void USB_Cable_Config (FunctionalState NewState)
{

  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}


/**
 *	@brief  Create the serial number string descriptor.
 *  @param  None
 *  @return None 
 */
void Get_SerialNum(void)
{
#ifdef USE_MSD_DRIVE
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &COMPOSITE_StringSerial[2] , 6);
    IntToUnicode (Device_Serial1, &COMPOSITE_StringSerial[18], 6);
  }
#endif /*USE_MSD_DRIVE*/
}

#ifdef USE_MSD_DRIVE

/**
 *	@brief  Convert Hex 32Bits value into char.
 *  @param  None
 *  @return None 
 */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

/**
 *	@brief  MAL_layer configuration
 *  @param  None
 *  @return None 
 */
void MAL_Config(void)
{
  MAL_Init(0);
}
#endif /*USE_MSD_DRIVE*/


/**
 *	@brief  Disconnect pin configuration
 *  @param  None
 *  @return None 
 */
void USB_Disconnect_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

  /* USB_DISCONNECT_PIN used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
}

/**
 *	@brief  Configures the Extern Interrupt for the IRQ coming from the CR95HF
 *  @param  None.
 *  @return None.
 */
void EXTI_Config( void )
{
	EXTI_DeInit( );
	EXTI_RCC_Config( );
	// GPIOA pin3 (as floating input)
	EXTI_GPIO_Config( );
	// the pin 14 of CR95HF can be configurate as interupt output
	EXTI_Structure_Config( ); 
}

/**
 *	@brief  Configures the Interrupt Output to send interuption to CR95HF
 *  @param  None.
 *  @return None.
 */
void IRQOut_Config( void )
{
	// GPIOA pin3 (as Open drain output)
	IRQOut_GPIO_Config( );
	// the pin 14 of CR95HF can be configurate as interupt output
	//EXTI_Structure_Config( ); 
}


/**
 *	@brief  Configures the SPI NSS pin to send a interuption pulse
 *  @param  None.
 *  @return None.
 */
void SPINSS_Config( void )
{
GPIO_InitTypeDef GPIO_InitStructure;

	// SPINSS as output push pull 
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   			= CR95HF_SPI_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_Out_PP; 
	GPIO_Init(CR95HF_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);
	
	// SPI_NSS  = High Level  
	GPIO_SetBits(CR95HF_SPI_NSS_GPIO_PORT, CR95HF_SPI_NSS_PIN);  

}

/**
 *	@brief  GPIO configuration for the EXTI
 *  @param  None.
 *  @return None.
 */
static void IRQOut_GPIO_Config( void )
{
GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure IRQ pin as open drain output */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin  = IRQOUT_CR95HF_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	CR95HF_IRQIN_HIGH() ;
}

/**
 *	@brief  RCC configuration for the EXTI
 *  @param  None.
 *  @return None.
 */
void EXTI_RCC_Config( void )
{
	/* Enable the Clock */
	RCC_APB2PeriphClockCmd(	EXTI_CR95HF_GPIO_CLOCK | 
							RCC_APB2Periph_AFIO, 
							ENABLE);
}

/**
 *	@brief  GPIO configuration for the EXTI
 *  @param  None.
 *  @return None.
 */
void EXTI_GPIO_Config( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure IRQ pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin  = EXTI_CR95HF_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* Connect IRQ EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(EXTI_CR95HF_GPIO_PORT_SOURCE, EXTI_CR95HF_PIN_SOURCE);
	EXTI_ClearITPendingBit(EXTI_CR95HF_LINE);
}

/**
 *	@brief  Structure configuration for the EXTI
 *  @param  None.
 *  @return None.
 */
void EXTI_Structure_Config( void )
{
EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure CR95HF IRQ EXTI line */
	EXTI_InitStructure.EXTI_Line 		= EXTI_CR95HF_LINE;
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;    
	EXTI_InitStructure.EXTI_LineCmd 	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/**
 *	@brief  Configures the Timer 2 (for standalone demo)
 *  @param  None.
 *  @return None.
 */
void Timer_Config( void )
{
	Timer_RCC_Config( );
	Timer_Structure_Config( );
}

/**
 *	@brief  RCC configuration for the Timer2
 *  @param  None.
 *  @return None.
 */
void Timer_RCC_Config( void )
{
	//enable TIM3 & TIM4
	RCC_APB1PeriphClockCmd(		TIMER_TIMEOUT_CLOCK 	|
								TIMER_DELAY_CLOCK 		, 
								ENABLE);
}

/**
 *	@brief  Structure configuration for the Timer2
 *  @param  None.
 *  @return None.
 */
void Timer_Structure_Config( void )
{
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* -------------------------------------------------------------------------- 
	 * TimeOut TIMER configuration
	 * -------------------------------------------------------------------------- 
	 * 72 MHz / 7200 = 10KHz (100탎)
	 * 100탎 * 300 + 100탎 ~= 30ms	
	 * -------------------------------------------------------------------------- */
	TIM_TimeBaseStructure.TIM_Period 			= TIMER_TIMEOUT_PERIOD;     
	TIM_TimeBaseStructure.TIM_Prescaler 		= TIMER_TIMEOUT_PRESCALER;       
	TIM_TimeBaseStructure.TIM_ClockDivision 	= TIM_CKD_DIV1;      
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Down;	  
	// Update the timeout timer (TIM3) 
	TIM_TimeBaseInit(TIMER_TIMEOUT, &TIM_TimeBaseStructure);
	
	TIM_UpdateRequestConfig(TIMER_TIMEOUT, TIM_UpdateSource_Global);
	
	TIM_ClearITPendingBit(TIMER_TIMEOUT, TIM_IT_Update);
		
	/* Enable TIMER Update interrupt */
	TIM_ITConfig(TIMER_TIMEOUT, TIM_IT_Update, ENABLE);
	
	
	/* -------------------------------------------------------------------------- 
	 * Delay TIMER configuration
	 * --------------------------------------------------------------------------
	 * 72 MHz / 72 = 1MHz (1탎)
	 * 1탎 * 1000 + 1탎 ~= 1ms	
	 * -------------------------------------------------------------------------- */ 
	TIM_TimeBaseStructure.TIM_Period 			= TIMER_DELAY_PERIOD;      
	TIM_TimeBaseStructure.TIM_Prescaler 		= TIMER_DELAY_PRESCALER;       
	TIM_TimeBaseStructure.TIM_ClockDivision 	= TIM_CKD_DIV1;    
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMER_DELAY, &TIM_TimeBaseStructure);
	
	TIM_UpdateRequestConfig(TIMER_DELAY, TIM_UpdateSource_Global);
	
	TIM_ClearITPendingBit(TIMER_DELAY, TIM_IT_Update);
		
	/* Enable TIMER Update interrupt */
	TIM_ITConfig(TIMER_DELAY, TIM_IT_Update, ENABLE);


	// Disable timers
	TIM_Cmd(TIMER_TIMEOUT, DISABLE);
	TIM_Cmd(TIMER_DELAY, DISABLE);

}


/**
 *	@brief  Interrupts configuration
|---------------|-----------------------|-------------------|-------------------|
|	 name		|	preemption priority	|	sub proiority	|	channel			|
|---------------|-----------------------|-------------------|-------------------|
| CR95HF		|		0				|		0			|	EXT9_5_IRQn		|
|---------------|-----------------------|-------------------|-------------------|
| USB_HP		|		1				|		0			|	USB_HP_CAN1_TX	|
|---------------|-----------------------|-------------------|-------------------|
| USB_LP		|		1				|		1			|	USB_LP_CAN1_TX	|
|---------------|-----------------------|-------------------|-------------------|
| timeout		|		2				|		0			|	TIM3_IRQn		|
|---------------|-----------------------|-------------------|-------------------|
| staandalone	|		2				|		1			|	TIM2_IRQn		|
|---------------|-----------------------|-------------------|-------------------|
| delay			|		2				|		2			|	TIM2_IRQn		|
|---------------|-----------------------|-------------------|-------------------|
 *  @param  None.
 *  @return None.
 */

/**
 *	@brief  Time delay in millisecond
 *  @param  delay : delay in ms.
 *  @return None.
 */
void delay_ms(uint16_t delay)
{
	counter_delay_ms = delay;

	TIM_SetCounter(TIMER_DELAY, 0);
	/* TIM4 enable counter */
  	TIM_Cmd(TIMER_DELAY, ENABLE);
	/* Wait for 'delay' milliseconds */
	while(counter_delay_ms != 0);
	/* TIM4 disable counter */
	TIM_Cmd(TIMER_DELAY, DISABLE);
}

/**
 *	@brief 	: Time delay in millisecond. The default priority are changed for this function.
 	@brief	: this function is used into void CR95HF_Send_SPI_ResetSequence(void).
 *  @param  delay : delay in ms.
 *  @return None.
 */
void delayHighPriority_ms(uint16_t delay)
{
NVIC_InitTypeDef NVIC_InitStructure;

	counter_delay_ms = delay;

	/* Enable and set TIMER IRQ used for delays. High priority*/ 
	NVIC_InitStructure.NVIC_IRQChannel 					 		= TIMER_DELAY_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 		= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 						= ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	TIM_ClearITPendingBit(TIMER_DELAY, TIM_IT_Update);
	TIM_SetCounter(TIMER_DELAY, 0);
	/* TIM4 enable counter */
  	TIM_Cmd(TIMER_DELAY, ENABLE);
	/* Wait for 'delay' milliseconds */
	while(counter_delay_ms != 0);
	/* TIM4 disable counter */
	TIM_Cmd(TIMER_DELAY, DISABLE);

	/* Enable and set TIMER IRQ used for delays. Default priority */ 
	NVIC_InitStructure.NVIC_IRQChannel 					 		= TIMER_DELAY_IRQ_CHANNEL;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= TIMER_DELAY_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 		= TIMER_DELAY_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd 						= ENABLE;
	NVIC_Init(&NVIC_InitStructure);	


}

/**
 *	@brief  decrement the counter every millisecond used by the function delay_ms
 *  @param  delay : delay in ms.
 *  @return None.
 */
void decrement_delay(void)
{
	if(counter_delay_ms != 0)
	{
		/* Decrement the counter */ 
		counter_delay_ms--;
	}
}

/**
 *	@brief  Starts the time out used to avoid the STM32 freeze
 *  @param  delay : delay in tenth of milliseconds (100탎).
 *  @return None.
 */
void StartTimeOut( uint16_t delay )
{
	/* Set the TimeOut flag to false */
	CR95HF_TimeOut 	 = FALSE;
	/* Clear the IT flag */
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	/* Set the timer counter */
	TIM_SetCounter(TIMER_TIMEOUT, delay);
  /* Enable the Time out timer */
	TIM_Cmd(TIMER_TIMEOUT, ENABLE);
}

/**
 *	@brief  Stop the timer used for the time out
 *  @param  None.
 *  @return None.
 */
void StopTimeOut( void )
{	
  	/* Disable the Time out timer */
	TIM_Cmd(TIMER_TIMEOUT, DISABLE);	
}

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
