/**
  ******************************************************************************
  * @file    drv_CR95HF.c 
  * @author  MMY Application Team
  * @version V1.3
  * @date    20/06/2011
  * @brief   This file provides set of firmaware functions to manage communication between MCU and CR95HF 
  * @brief   this file applies to Demo board CR95HF
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

/* Includes ------------------------------------------------------------------------------ */
#include "lib_CR95HF.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "hw_config.h"
#include "string.h"
#include "drv_spi.h"
#include "drv_uart.h"
#include "stm32f10x_usart.h"
#include "drv_LED.h"
#include "miscellaneous.h"
#include "usb_type.h"

/** @addtogroup Drivers
 * 	@{
 */

/** @addtogroup CR95HF
 * 	@{
 */



/** @defgroup drvCR95HF_Private_Variables
 * @{
 */

/** 
 * @brief Allows you to know when data are available in the CR95HF buffer
 */ 
__IO bool								CR95HF_DataReady;
/** 
 * @brief Avoids to stuck the program
 */
__IO bool								CR95HF_TimeOut;
/** 
 * @brief Contains the CR95HF interface (SPI or UART)
 */
__IO CR95HF_INTERFACE	CR95HF_Interface;

/** @defgroup drvCR95HF_External_Variables
 * @{
 */
 
/** 
 * @brief ConfigStructure
 */ 										 
extern ReaderConfigStruct			ReaderConfig;

/**
 * @}
 */

/** @defgroup drvCR95HF_Private_Functions
 *  @{
 */
static void CR95HF_RCC_Config_SPI			(void);
static void CR95HF_GPIO_Config_SPI			(void);
static void CR95HF_Structure_Config_SPI		(void);
static void CR95HF_RCC_Config_UART			(void);
static void CR95HF_GPIO_Config_UART			(void);
static void CR95HF_Send_SPI_ResetByte		(void);

static void 	SPI_Initialization			( void );


/** @defgroup drvCR95HF_Functions
 *  @{
 */

/**
 *	@brief  Initializes clock
 *  @param  void
 *  @retval void 
 */
static void CR95HF_RCC_Config_SPI(void)
{	
	// Enable GPIO clock 
  RCC_APB2PeriphClockCmd( 	CR95HF_SPI_SCK_GPIO_CLK  | 
							CR95HF_SPI_MISO_GPIO_CLK | 
							CR95HF_SPI_MOSI_GPIO_CLK , 
							ENABLE);
	
  // Enable SPI clock  
  RCC_APB2PeriphClockCmd(CR95HF_SPI_CLK, ENABLE);
}

/**
 *	@brief  Initializes GPIO for SPI communication
 *  @param  None
 *  @retval None 
 */
static void CR95HF_GPIO_Config_SPI(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure SPI pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin 			= CR95HF_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 			= GPIO_Mode_AF_PP;
	GPIO_Init(CR95HF_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 			= CR95HF_SPI_MOSI_PIN;
	GPIO_Init(CR95HF_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);


//	GPIO_InitStructure.GPIO_Mode 			= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode 			= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin 			= CR95HF_SPI_MISO_PIN;
	GPIO_Init(CR95HF_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure I/O for Chip select */		
	GPIO_InitStructure.GPIO_Pin   			= CR95HF_SPI_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_Out_PP; 
	GPIO_Init(CR95HF_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);
	
	// SPI_NSS  = High Level  
	GPIO_SetBits(CR95HF_SPI_NSS_GPIO_PORT, CR95HF_SPI_NSS_PIN);  
}

/**
 *	@brief  SET SPI protocol
 *  @param  None
 *  @retval None 
 */ 
static void CR95HF_Structure_Config_SPI( void )
{
SPI_InitTypeDef  SPI_InitStructure;

	/* Initialize the SPI with default values */
	SPI_StructInit(&SPI_InitStructure);

	/* SPI Config master with NSS manages by software using the SSI bit*/
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;

	SPI_InitStructure.SPI_NSS  				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;

	/* Init the SPI BRIDGE */
	SPI_Init(CR95HF_SPI, &SPI_InitStructure);

 	/* Enable SPI */
	SPI_Cmd(CR95HF_SPI, ENABLE);
}

/**
 *	@brief  this functions initializes the SPI in order to communicate with the CR95HF
 *  @param  void
 *  @retval void 
 */
static void SPI_Initialization(void)
{
	SPI_I2S_DeInit(CR95HF_SPI);
	//enables SPI and GPIO clocks
	CR95HF_RCC_Config_SPI( );
   // configures GPIO A PA4-7 as SPI bus  (NSS = HL)
	CR95HF_GPIO_Config_SPI( );	
	CR95HF_Structure_Config_SPI( );
}



/**
 *	@brief  Initializes clock	for UART
 *  @param  None
 *  @retval None 
 */
static void CR95HF_RCC_Config_UART(void)
{	
	//Enable GPIO clock
	RCC_APB2PeriphClockCmd( 	CR95HF_UART_TX_GPIO_CLK | 
						  	CR95HF_UART_RX_GPIO_CLK, /*| 
							RCC_APB2Periph_AFIO, 	  */
							ENABLE);
	//enable USART 2 clock
	RCC_APB1PeriphClockCmd(		CR95HF_UART_CLK , 
							ENABLE);
							   
}

/**
 *	@brief  Initializes GPIO for UART configuration
 *  @param  None
 *  @retval None 
 */
static void CR95HF_GPIO_Config_UART(void)
{
GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USART Tx as ouput alternate function push pull*/
	GPIO_InitStructure.GPIO_Pin 	= CR95HF_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_Init(CR95HF_UART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = CR95HF_UART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(CR95HF_UART_RX_GPIO_PORT, &GPIO_InitStructure); 
}


/**
 *	@brief  this functions initializes UART in order to communicate with CR95HF
 *  @param  None
 *  @retval None 
 */
void UART_Initialization(uc32 BaudRate)
{
	USART_DeInit(CR95HF_UART);
	// enables clock
	CR95HF_RCC_Config_UART( );
	// configures RX and TX 
	// TX : alternate function push pull
	// RX : floating input 
	CR95HF_GPIO_Config_UART( );
	// set protcol 
	CR95HF_Structure_Config_UART(BaudRate );
}

/**
 *	@brief  set protocol 
 *  @param  None
 *  @retval None 
 */ 
void CR95HF_Structure_Config_UART(uc32 BaudRate)
{
	USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate 			= BaudRate;
  USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
  USART_InitStructure.USART_StopBits 			= USART_StopBits_1;
  USART_InitStructure.USART_Parity 				= USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode 				= USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART */
  USART_Init(CR95HF_UART, &USART_InitStructure);
  
  /* Enable the USART */
  USART_Cmd(CR95HF_UART, ENABLE);
}



/**  
* @brief  	initilize the CR95HF config structure
* @param  	
* @retval 	CR95HF_SUCCESS_CODE 	: Ok
*/
int8_t InitCR95HFConfigStructure (ReaderConfigStruct *ReaderConfig)
{
	ReaderConfig->Interface = CR95HF_INTERFACE_SPI;
	ReaderConfig->SpiMode = SPI_POLLING;
	ReaderConfig->State = CR95HF_STATE_POWERUP;
  	return CR95HF_SUCCESS_CODE;
}

/**
 *	@brief  Send a negative pulse on IRQin pin
 *  @param  none
 *  @return None
 */
void CR95HF_Send_IRQIN_NegativePulse(void)
{
	if (ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
	{
		CR95HF_IRQIN_HIGH() ;
		//delay_ms(1);
		delayHighPriority_ms(1);
		CR95HF_IRQIN_LOW() ;
		//delay_ms(1);
		delayHighPriority_ms(1);
		CR95HF_IRQIN_HIGH() ;
	}
	else if (ReaderConfig.Interface == CR95HF_INTERFACE_UART)
	{
	 	UART_SendByte(CR95HF_UART, 0x00);
	}
}

/**
 *	@brief  Send a negative pulse on SPI_NSS pin
 *  @param  none
 *  @return None
 */
void CR95HF_Send_SPINSS_NegativePulse(void)
{
	if (ReaderConfig.Interface == CR95HF_INTERFACE_SPI ||
		ReaderConfig.Interface == CR95HF_INTERFACE_UART )
	{	
		CR95HF_NSS_HIGH() ;
		delayHighPriority_ms(1);
		CR95HF_NSS_LOW() ;
		delayHighPriority_ms(1);
		CR95HF_NSS_HIGH() ;
	}
}


/**
 *	@brief  Send a reset command over SPI bus
 *  @param  none
 *  @return None
 */
static void CR95HF_Send_SPI_ResetByte(void)
{
	// Send reset control byte
	SPI_SendReceiveByte(CR95HF_SPI, CR95HF_COMMAND_RESET);
}


/**
 *	@brief  Send a reset sequence over SPI bus (Reset command + wait 5µs + 
 *	@brief  negative pulse on IRQin.
 *  @param  none
 *  @return None
 */
void CR95HF_Send_SPI_ResetSequence(void)
{
	if (ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
	{
		// Select CR95HF over SPI 
		CR95HF_NSS_LOW();
		// Send reset control byte
		CR95HF_Send_SPI_ResetByte();
		// Deselect CR95HF over SPI 
		CR95HF_NSS_HIGH();
		delayHighPriority_ms(3);
		// send a pulse on IRQ_in
		CR95HF_Send_IRQIN_NegativePulse();
		delayHighPriority_ms(10);
	}
}


/**
 *	@brief  returns the value of interface pin. 
 *	@brief  Low level means UART bus was choose.
 *	@brief  High level means SPI bus was choose.
  *  @param  none
 *  @return Bit_RESET 	: 0
 *  @return Bit_SET 	: 1
 */
int8_t CR95HF_GetInterfacePinState (void)
{
	
	//return CR95HF_GET_INTERFACE();
	return CR95HF_INTERFACE_SPI;
	//return CR95HF_INTERFACE_UART;
}

/**
 *	@brief  This function initialize MCU serial interface peripheral (SPI or UART)
 *  @param  none
 *  @return void
 */
void CR95HF_SerialInterfaceInitilisation ( void )
{
	// get interface pin state. UART or SPI
	if (CR95HF_GetInterfacePinState () == CR95HF_INTERFACE_UART)
		ReaderConfig.Interface = CR95HF_INTERFACE_UART;
	
	// configures the SPI or UART bus
	if (ReaderConfig.Interface == CR95HF_INTERFACE_UART)
	{	UART_Initialization(BAUDRATE_DATARATE_DEFAULT );	 
		// -- SPI_NSS configuration to send a pulse 
		SPINSS_Config( );
	}
	
	else if (ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
	{
		ReaderConfig.Interface = CR95HF_INTERFACE_SPI;
		SPI_Initialization( );
		// -- IRQout configuration PA2 to send pulse on USART_RX of CR95HF
		IRQOut_Config( );
		// -- EXTI configuration if the polling mode is not selected
		if (ReaderConfig.SpiMode == SPI_INTERRUPT) //SPI_POLLING_MODE 
			// -- An EXTI must be set to catch the IRQ (a edge on the UART RX pin) from the CR95HF
			EXTI_Config( );	
	}
}

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

