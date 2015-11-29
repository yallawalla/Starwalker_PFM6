/**
  ******************************************************************************
  * @file    lib_CR95HF.c 
  * @author  MMY Application Team
  * @version V1.4
  * @date    12/06/2012
  * @brief   This file provides set of firmaware functions to manages CR95HF device. 
  * @brief   The commands as defined in CR95HF datasheet
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
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------------------ */
#include "lib_CR95HF.h"
#include "hw_config.h"
#include "string.h"
#include "drv_spi.h"
#include "drv_uart.h"
#include "miscellaneous.h"
#include "usb_lib.h"
#include "ff.h"
#include "mass_mal.h"
#include "usb_scsi.h"

#include "lib_iso15693.h"


/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup CR95HF
 * 	@{
 */



/** @defgroup CR95HF_Extern_Variables
 *  @{
 */	
extern volatile bool										CR95HF_DataReady;
extern volatile bool										CR95HF_TimeOut;
extern volatile CR95HF_INTERFACE				CR95HF_Interface;

extern ReaderConfigStruct								ReaderConfig;
extern uint8_t													TagMemoryBuffer[8192];
extern uint8_t													ReaderRecBuf [0xFF];

extern bool HID_TRANSACTION;

/*Used by CR95HF_Send_Trame*/
uint8_t Glo_NbHIDTrame;


#ifdef USE_MSD_DRIVE
FATFS fs;         /* Work area (file system object) for logical drives */
FIL fdst;      /* file objects */
FRESULT res;
UINT bw, br;
#endif /*USE_MSD_DRIVE*/


#ifdef USE_MSD_DRIVE
FATFS ff;         /* Work area (file system object) for logical drives */
FIL fdft;      /* file objects */
extern FRESULT res;
extern UINT bw, br;
#endif /*USE_MSD_DRIVE*/


static const uint8_t MCUVERSION[MCUVERSION_LENGTH] =
{	MCUVERSION_FIRSTNUMBER,
  MCUVERSION_SECONDNUMBER,
	MCUVERSION_THIRDNUMBER
};




/**
  * @}
  */ 



static void CR95HF_Send_SPI_Command			(uc8 *pData);
static int8_t CR95HF_PollingCommand			(void);
static void CR95HF_Send_UART_Command		(uc8 *pData);
static int8_t GetNbControlByte 				(int8_t ProtocolSelected);

static uint8_t IsAnAvailableProtocol 		(uint8_t Protocol);
static uint8_t IsAnAvailableSelectLength 	(uc8 Protocol,uc8 Length);
static uint8_t IsAnAvailableSelectParameters (uc8 Protocol,uc8 Length,uc8 *parameters) ;
static uint8_t ForceSelectRFUBitsToNull 	(uc8 Protocol,uc8 Length,uint8_t *parameters);
//static int8_t CR95HF_ReadTagSectorMemory 	(uc8 *pCommand, uint8_t *pResponse );
//static int8_t CR95HF_DownloadSTM32Memory 	(uc8 *pCommand, uint8_t *pResponse );

static int8_t CR95HF_GotoTagDetectingState	(uc8 *pCommand, uint8_t *pResponse );
static int8_t CR95HF_IsWakeUp				(uc8 *pCommand, uint8_t *pResponse );

static int8_t CR95HF_ReadAllMemory (uc8 *pCommand, uint8_t *pResponse );
static void CR95HF_ReadMCUBuffer (uc8 *pCommand, uint8_t *pResponse );

#ifdef USE_MSD_DRIVE

static uint8_t CR95HF_CopyTagToFile (uc8 *pCommand, uint8_t *pResponse);
static uint8_t CR95HF_CopyFileToTag (uc8 *pCommand, uint8_t *pResponse);


#endif /*USE_MSD_DRIVE*/

static int8_t CR95HF_Tag_Traking ( uc8 *pCommand, uint8_t *pResponse );
static int8_t CR95HF_Por ( uc8 *pCommand, uint8_t *pResponse );
static int8_t CR95HF_GetTagDetectionRefValue	( uint8_t *DacDataRef );
static int8_t CR95HF_IsWakeUpByTagDetecting	( uint8_t *IdleResponse );
static int8_t CR95HF_IsWakeUpByTimeOut			( uint8_t *IdleResponse );



/**
 *	@brief  this functions returns CR95HF_SUCCESS_CODE if the protocol is available, otherwise CR95HF_ERRORCODE_PARAMETER
 *  @param  Protocol : RF protocol (ISO 14443 A or 14443 B or 15 693 or Fellica)
 *  @return CR95HF_SUCCESS_CODE	: the protocol is available
 *  @return CR95HF_ERRORCODE_PARAMETER : the protocol isn't available
 */
static uint8_t IsAnAvailableProtocol (uint8_t Protocol) 
{
	switch(Protocol)
	{
		case PROTOCOL_TAG_FIELDOFF:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO15693:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443A:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443B:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_FELICA:
			return CR95HF_SUCCESS_CODE;
		default: return CR95HF_ERRORCODE_PARAMETER;
	}	
}

/**
 *	@brief  this functions returns CR95HF_SUCCESS_CODE if length value is allowed, otherwise ERRORCODE_GENERIC
 *	@brief  this functions is used by ProtocolSelect function.
 *  @param  Protocol : RF protocol (ISO 14443 A or B or 15 693 or Fellica)
 *  @param  Length : Number of byte of Protocol Select command
 *  @return CR95HF_SUCCESS_CODE	: the length value is correct
 *  @return ERRORCODE_GENERIC : the length value isn't correct
 */
static uint8_t IsAnAvailableSelectLength (uint8_t Protocol,uint8_t Length) 
{
	switch(Protocol)
	{
		case PROTOCOL_TAG_FIELDOFF:
			if (Length == 2 )
				return CR95HF_SUCCESS_CODE;
			else return ERRORCODE_GENERIC;
		case PROTOCOL_TAG_ISO15693:
			if (Length == 2 )
				return CR95HF_SUCCESS_CODE;
			else return ERRORCODE_GENERIC;
		case PROTOCOL_TAG_ISO14443A:
			// length == 2 protocol + parameters 
			// length == 4 protocol + parameters + AFDT (2 bytes)
			if (Length == 2 || Length == 4 )
				return CR95HF_SUCCESS_CODE;
			else return CR95HF_ERRORCODE_PARAMETER;
		case PROTOCOL_TAG_ISO14443B:
			if (Length == 2 || Length == 4 )
				return CR95HF_SUCCESS_CODE;
			else return CR95HF_ERRORCODE_PARAMETER;
		case PROTOCOL_TAG_FELICA:
			if (Length == 2 )
				return CR95HF_SUCCESS_CODE;
			else return CR95HF_ERRORCODE_PARAMETER;
		default: return CR95HF_ERRORCODE_PARAMETER;
	}	
}

/**
 *	@brief  this functions returns CR95HF_SUCCESS_CODE if parameter value is correct, otherwise ERRORCODE_GENERIC
 *	@brief  this functions is used by ProtocolSelect function.
 *  @param  Protocol : RF protocol (ISO 14443 A or B or 15 693 or Fellica)
 *  @param  Length : Number of byte of parameters
 *  @param  parameters : pointer on parameter of ProtocolSelect parameter
 *  @return CR95HF_SUCCESS_CODE	: the parameter value is correct
 *  @return CR95HF_ERRORCODE_PARAMETER : the length value isn't correct
 */
static uint8_t IsAnAvailableSelectParameters (uc8 Protocol,uc8 Length,uc8 *parameters) 
{
	switch(Protocol)
	{
		case PROTOCOL_TAG_FIELDOFF:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO15693:
			if ((parameters[0] & 0x30) == 0x30)
				return CR95HF_ERRORCODE_PARAMETER;
			else return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443A:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443B:
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_FELICA:
			if ( ((parameters[0] & 0x30) == 0x30) || ((parameters[0] & 0x30) == 0x00))
				return CR95HF_ERRORCODE_PARAMETER;
			return CR95HF_SUCCESS_CODE;
		default : return CR95HF_ERRORCODE_PARAMETER;
	}	
}

/**
 *	@brief  this functions reset RFU bits of parameters
 *  @param  Protocol : RF protocol (ISO 14443 A or B or 15 693 or Fellica)
 *  @param  Length : Number of byte of parameters
 *  @param  parameters : pointer on parameter of ProtocolSelect parameter 
 *  @return CR95HF_SUCCESS_CODE	: RFU bits was reset
 *  @return CR95HF_ERRORCODE_PARAMETER : the protocol isn't available
 */
static uint8_t ForceSelectRFUBitsToNull (uc8 Protocol,uc8 Length,uint8_t *parameters) 
{
	
	switch(Protocol)
	{
		case PROTOCOL_TAG_FIELDOFF:
			parameters[0]= 0x00;
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO15693:
			// bit 7:6 RFU
			parameters[0] &= 0x3F;
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443A:
			//bits 2:0 of byte 0 defined as RFU
			parameters[Length-2] &= 0xF8;
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_ISO14443B:
			//bits 3:1 of byte 0 defined as RFU
			parameters[Length-2] &= 0xF1;
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_TAG_FELICA:
			// bits 3:1 & 7:6 of byte 0 defined as RFU
			parameters[1] &= 0x31;
			parameters[0] &= 0x1F;
			return CR95HF_SUCCESS_CODE;
		default : return CR95HF_ERRORCODE_PARAMETER;
	}	
}


/**
 *	@brief  this function send a command over SPI bus
 *  @param  *pData : pointer on data to send to the CR95HF
 *  @return None
 */
static void CR95HF_Send_SPI_Command(uc8 *pData)
{
uint8_t DummyBuffer[MAX_BUFFER_SIZE];
	  
	// Select CR95HF over SPI 
	CR95HF_NSS_LOW();

	// Send a sending request to CR95HF 
	SPI_SendReceiveByte(CR95HF_SPI, CR95HF_COMMAND_SEND);

	if(*pData == ECHO)
	{
		// Send a sending request to CR95HF
		SPI_SendReceiveByte(CR95HF_SPI, ECHO);
	}
	else
	{
		// Transmit the buffer over SPI
		SPI_SendReceiveBuffer(CR95HF_SPI, pData, pData[CR95HF_LENGTH_OFFSET]+CR95HF_DATA_OFFSET, DummyBuffer);
	}

	//Deselect CR95HF over SPI 
	CR95HF_NSS_HIGH();
}


/**
 *	@brief  this function send polling control byte 
 *  @param  None
 *  @return None
 */
static int8_t CR95HF_PollingCommand( void )
{
uint8_t Polling_Status = 0;

	StartTimeOut(30000);		

	if (ReaderConfig.SpiMode == SPI_POLLING)
	{
		
		while( Polling_Status 	!= CR95HF_FLAG_DATA_READY && 
					 CR95HF_TimeOut != TRUE )
		{
			CR95HF_NSS_LOW();
			// Send a polling request to CR95HF 
			SPI_SendReceiveByte(CR95HF_SPI, CR95HF_COMMAND_POLLING);
	
			// poll the CR95HF until he's ready ! 
			Polling_Status  = SPI_SendReceiveByte(CR95HF_SPI, CR95HF_COMMAND_POLLING);
			Polling_Status &= CR95HF_FLAG_DATA_READY_MASK;
	
			// Pulse on NSS 
			CR95HF_NSS_HIGH();		
		}
	}	
	else if (ReaderConfig.SpiMode == SPI_INTERRUPT)
	{
		// reset the CR95HF data status 
		CR95HF_DataReady = FALSE;
		
		// Enable Interrupt on the falling edge on the IRQ pin of CR95HF 
		EXTI_ClearITPendingBit(EXTI_CR95HF_LINE);
		EXTI->IMR |= EXTI_CR95HF_LINE;

		// Wait a low level on the IRQ pin or the timeout 
		//while( (CR95HF_DataReady != TRUE) && CR95HF_TimeOut != TRUE );
		while( (CR95HF_DataReady == FALSE) & (CR95HF_TimeOut == FALSE) )
		{	}

		if(CR95HF_TimeOut == TRUE)
		{		
			// Disable CR95HF EXTI 
			EXTI->IMR &= ~EXTI_CR95HF_LINE;
		}
	}

	StopTimeOut( );

	if ( CR95HF_TimeOut == TRUE )
		return CR95HF_POLLING_TIMEOUT;

	return CR95HF_SUCCESS_CODE;	
}

/**
 *	@brief  this fucntion recovers a response from CR95HF device
 *  @param  *pData : pointer on data received from CR95HF device
 *  @return None
 */
void CR95HF_Receive_SPI_Response(uint8_t *pData)
{
uint8_t DummyBuffer[MAX_BUFFER_SIZE];

	// Select CR95HF over SPI 
	CR95HF_NSS_LOW();

	// Request a response from CR95HF 
	SPI_SendReceiveByte(CR95HF_SPI, CR95HF_COMMAND_RECEIVE);

	// Recover the "Command" byte 
	pData[CR95HF_COMMAND_OFFSET] = SPI_SendReceiveByte(CR95HF_SPI, DUMMY_BYTE);

	if(pData[CR95HF_COMMAND_OFFSET] == ECHO)
		 pData[CR95HF_LENGTH_OFFSET]  = 0x00;
	else if(pData[CR95HF_COMMAND_OFFSET] == 0xFF)
		pData[CR95HF_LENGTH_OFFSET]  = 0x00;
	else
	{
		// Recover the "Length" byte 
		pData[CR95HF_LENGTH_OFFSET]  = SPI_SendReceiveByte(CR95HF_SPI, DUMMY_BYTE);
		// Checks the data length 
		if(pData[CR95HF_LENGTH_OFFSET] != 0x00)
			// Recover data 	
			SPI_SendReceiveBuffer(CR95HF_SPI, DummyBuffer, pData[CR95HF_LENGTH_OFFSET], &pData[CR95HF_DATA_OFFSET]);
		
	}

	//. Deselect CR95HF over SPI 
	CR95HF_NSS_HIGH();

}

/**												   
 *	@brief  this function send a command to CR95HF device over SPI bus
 *  @param  *pData : pointer on data to send to the CR95HF
 *  @return None
 */
static void CR95HF_Send_UART_Command(uc8 *pData)
{
	if(pData[0] == ECHO)
		// send Echo 
		UART_SendByte(CR95HF_UART, ECHO);
	
	else
		// send the buffer over UART 
		UART_SendBuffer(CR95HF_UART, pData, pData[CR95HF_LENGTH_OFFSET] + CR95HF_DATA_OFFSET);
	
}

/**
 *	@brief  this functions recovers a response from CR95HF device over SPI bus
 *  @param  *pData : pointer on data received from CR95HF device
 *  @return None
 */
static void CR95HF_Receive_UART_Response(uint8_t *pData)
{
	// Recover the "Command" byte 
	pData[CR95HF_COMMAND_OFFSET] = UART_ReceiveByte(CR95HF_UART);

	if(pData[CR95HF_COMMAND_OFFSET] == ECHO)
		pData[CR95HF_LENGTH_OFFSET]  = 0x00;
	
	else
	{
		// Recover the "Length" byte 
		pData[CR95HF_LENGTH_OFFSET]  = UART_ReceiveByte(CR95HF_UART);
	
		// Recover data
		if(pData[CR95HF_LENGTH_OFFSET]	!= 0)
			UART_ReceiveBuffer(CR95HF_UART, &pData[CR95HF_DATA_OFFSET], pData[CR95HF_LENGTH_OFFSET]);
	}
}

/**
 *	@brief  This function checks the baud rate is compatible with this MCU and configurates MCU and CR95HF baud rate.
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @retval CR95HF_SUCCESS_CODE : the baud rate is compatible
 *  @retval CR95HF_ERRORCODE_DEFAULT : the baud rate is not compatible
 */
static int8_t CR95HF_IsUartMCUAndCR95HFAvailable(uc8 *pCommand)
{
	if (pCommand[CR95HF_COMMAND_OFFSET] != BAUD_RATE) 
		return CR95HF_ERRORCODE_DEFAULT;
	
	
	switch (pCommand[CR95HF_DATA_OFFSET])
	{
		case 0 : // RFU in data sheet
			return CR95HF_ERRORCODE_DEFAULT;
		case 1 :  // RFU in data sheet
			return CR95HF_ERRORCODE_DEFAULT;
		case 2 :  // not compatible with MCU
			return CR95HF_ERRORCODE_DEFAULT;	
		case 5 :  // not compatible with MCU
			return CR95HF_ERRORCODE_DEFAULT;	
		case 8 :  // not compatible with MCU
			return CR95HF_ERRORCODE_DEFAULT;

		case 0x75 :  
			return CR95HF_SUCCESS_CODE;

		default : return CR95HF_ERRORCODE_DEFAULT; 	
	}  	
}

/**
 *	@brief  This function computes the UART baud rate according to Baudrate command parameter
 *  @param  BaudRateCommandParameter  : Baudrate command parameter (1 byte)
 *  @retval the UART baud rate value
 */
static uint32_t CR95HF_ComputeUARTBaudRate (uc8 BaudRateCommandParameter)
{
	return (13.56e6/ (2*BaudRateCommandParameter+2));
}

/**  
* @brief  	this function returns the number of control byte according to RF protocol
* @param  	ProtocolSelected : Rf protocol selected
* @retval 	CONTROL_15693_NBBYTE 	: number of control byte for 15 693 protocol
* @retval 	CONTROL_14443A_NBBYTE	: number of control byte for 14 443 A protocol
* @retval 	CONTROL_14443B_NBBYTE  	: number of control byte for 14 443 B protocol
* @retval 	CONTROL_FELICA_NBBYTE  	: number of control byte for Felica protocol
* @retval 	0 : error the protocol in unknown
*/
static int8_t GetNbControlByte (int8_t ProtocolSelected)
{

	switch(ProtocolSelected)
	{
		case PROTOCOL_TAG_ISO15693:
			return CONTROL_15693_NBBYTE;
		case PROTOCOL_TAG_ISO14443A:
			return CONTROL_14443A_NBBYTE;
		case PROTOCOL_TAG_ISO14443B:
			return CONTROL_14443B_NBBYTE;
		case PROTOCOL_TAG_FELICA:
			return CONTROL_FELICA_NBBYTE;
		default :
			return 0;
	}
}


/** @defgroup CR95HF_Functions
 *  @{
 */
/** @defgroup CR95HF_CommandFunctions
*  @brief  this set of function send a command defined in CR95HF datasheet to a CR95HF device 
 *  @{
 */

/**
 *	@brief  this function send an IDN command to CR95HF device
 *  @param  pResponse : pointer on CR95HF reply
 *  @retval CR95HF_SUCCESS_CODE : the function is succesful 
 */
int8_t CR95HF_IDN(uint8_t *pResponse)
{
uc8 DataToSend[] = {IDN	,0x00};
	
	SPIUART_SendReceive(DataToSend, pResponse);

return CR95HF_SUCCESS_CODE;

}


#ifdef USE_POLL_FIELD
/**
 *	@brief  this function send an IDN command to CR95HF device
 *  @param  pResponse : pointer on CR95HF reply
 *  @retval CR95HF_SUCCESS_CODE : the function is succesful 
 */
int8_t CR95HF_POLL_FIELD(uint8_t *pResponse)
{
uc8 DataToSend[] = {POLL_FIELD	,0x00};
	
	SPIUART_SendReceive(DataToSend, pResponse);

return CR95HF_SUCCESS_CODE;

}
#endif /*USE_POLL_FIELD*/

/**
 *	@brief  this function send an EOF pulse to a contacless tag
 *  @param  pResponse : pointer on CR95HF reply
 *  @retval CR95HF_SUCCESS_CODE : the function is succesful 
 */
int8_t CR95HF_SendEOF(uint8_t *pResponse)
{
uc8 DataToSend[] = {SEND_RECEIVE	,0x00};

	SPIUART_SendReceive(DataToSend, pResponse);

return CR95HF_SUCCESS_CODE;
}



/**
 *	@brief  this functions turns the field off
 *  @param  none
 *  @retval CR95HF_SUCCESS_CODE : the function is succesful 
 */
void CR95HF_FieldOff( void )
{
uc8 	DataToSend[] = {PROTOCOL_SELECT,0x02,0x00,0x00};
uint8_t pResponse[10];

	SPIUART_SendReceive(DataToSend, pResponse);

}

/**
 * @brief  this functions turns the field on
 * @param  none
 * @return none
 */
void CR95HF_FieldOn ( void )
{
//uc8 FIELD_ON[] =	{WRITE_REGISTER, 0x04, 0x68, 0x01, 0x07, 0x00};
uc8 FIELD_ON[] =	{PROTOCOL_SELECT, 0x02, 0x01, 0x00};
uint8_t pResponse[10];

		CR95HF_CheckSendReceive(FIELD_ON, pResponse);
		

}



/**
 *	@brief  this function send Echo command to CR95HF 
 *  @param  pResponse : pointer on CR95HF reply
 *  @return CR95HF_SUCCESS_CODE 
 */
int8_t CR95HF_Echo(uint8_t *pResponse)
{
uc8 command[]= {ECHO};

	SPIUART_SendReceive(command, pResponse);

return CR95HF_SUCCESS_CODE;

}

/**
 *	@brief  this function send a ProtocolSeclect command to CR95HF
 *  @param  Length  : number of byte of protocol select command parameters
 *  @param  Protocol : RF protocol (ISO 14443 A or B or 15 693 or Fellica)
 *  @param  Parameters: prococol parameters (see reader datasheet)
 *  @param  pResponse : pointer on CR95HF response
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : the Length parameter is erroneous
 *  @return CR95HF_ERRORCODE_PARAMETER : a parameter is erroneous
 */
int8_t CR95HF_ProtocolSelect(uc8 Length,uc8 Protocol,uc8 *Parameters,uint8_t *pResponse)
{
uint8_t DataToSend[SELECT_BUFFER_SIZE],
		SelectParameters[SELECT_BUFFER_SIZE];
int8_t	status,
		i=0; 

	if (CHECKVAL(Length,1,SELECT_BUFFER_SIZE) == FALSE)
		return CR95HF_ERRORCODE_PARAMETERLENGTH;

	// check the function parameters
	errchk(IsAnAvailableProtocol(Protocol));
	errchk(IsAnAvailableSelectLength(Protocol,Length));
	errchk(IsAnAvailableSelectParameters(Protocol,Length,Parameters));

	memcpy(SelectParameters,Parameters,Length);

	// force the RFU bits to 0
	errchk(ForceSelectRFUBitsToNull	(Protocol,Length,SelectParameters));

	DataToSend[CR95HF_COMMAND_OFFSET ] = PROTOCOL_SELECT;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= Length;
	DataToSend[CR95HF_DATA_OFFSET    ]	= Protocol;

	// DataToSend CodeCmd Length Data
	// Parameters[0] first byte to emmit
	for (i = 0; i< Length -1  ;i++)
		DataToSend[CR95HF_DATA_OFFSET +1 + i ] = SelectParameters[i];

  	SPIUART_SendReceive(DataToSend, pResponse);

	return CR95HF_SUCCESS_CODE;

Error:
	// initialize the result code to 0xFF and length to in case of error 
	*pResponse = CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;
	return CR95HF_ERRORCODE_PARAMETER;	
}


/**
 *	@brief  this function send a SendRecv command to CR95HF. the command to contacless device is embeded in Parameters.
 *  @param  Length 		: Number of bytes
 *  @param	Parameters 	: data depenps on protocl selected
 *  @param  pResponse : pointer on CR95HF response
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 *  @return CR95HF_ERROR_CODE : CR95HF returned an error code
 *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : Length parameter is erroneous
 */
int8_t CR95HF_SendRecv(uc8 Length,uc8 *Parameters,uint8_t *pResponse)
{
uint8_t DataToSend[SENDRECV_BUFFER_SIZE];

	// initialize the result code to 0xFF and length to 0
	*pResponse = CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;
	

	// check the function parameters
	if (CHECKVAL (Length,1,255)==FALSE)
		return CR95HF_ERRORCODE_PARAMETERLENGTH; 

	DataToSend[CR95HF_COMMAND_OFFSET ] = SEND_RECEIVE;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= Length;

	// DataToSend CodeCmd Length Data
	// Parameters[0] first byte to emmit
	memcpy(&(DataToSend[CR95HF_DATA_OFFSET]),Parameters,Length);


	SPIUART_SendReceive(DataToSend, pResponse);


	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) != CR95HF_SUCCESS_CODE)
		return CR95HF_ERROR_CODE;

	return CR95HF_SUCCESS_CODE;
}

/**
 *	@brief  this function send a Send command to CR95HF. the command to contacless device is embeded in Parameters.
 *  @param  Length 		: Number of bytes
 *  @param	Parameters 	: data depenps on protocl selected
  *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
  *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : Length parameter is erroneous
 */
int8_t CR95HF_Send(uc8 Length,uc8 *Parameters)
{
uint8_t DataToSend[SENDRECV_BUFFER_SIZE];

	// check the function parameters
	if (CHECKVAL (Length,1,255)==FALSE)
		return CR95HF_ERRORCODE_PARAMETERLENGTH; 

	DataToSend[CR95HF_COMMAND_OFFSET ] = SEND;
	DataToSend[CR95HF_LENGTH_OFFSET  ] = Length;

	// DataToSend CodeCmd Length Data
	// Parameters[0] first byte to emmit
	memcpy(&(DataToSend[CR95HF_DATA_OFFSET]),Parameters,Length);
//	for (i =0 ; i< Length ;i++)
//		DataToSend[CR95HF_DATA_OFFSET + i ] = Parameters[i];

	SPIUART_Send (DataToSend);

	return CR95HF_SUCCESS_CODE;
}

/**
 *	@brief  this function send a Idle command to CR95HF
 *  @param  Length 	: Number of bytes
 *  @param	Data	: pointer on data. Data depends on protocl selected
 *  @param  pResponse : pointer on CR95HF response
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 *  @return CR95HF_ERROR_CODE : CR95HF returned an error code
 *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : Length parameter is erroneous
 */
int8_t CR95HF_Idle( uc8 Length, uc8 *Data )
{
uint8_t DataToSend[IDLE_BUFFER_SIZE];

	// check the function parameters
	if (Length != 0x0E)
		return CR95HF_ERRORCODE_PARAMETERLENGTH; 

	DataToSend[CR95HF_COMMAND_OFFSET ] = IDLE;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= Length;

	
	memcpy(&(DataToSend[CR95HF_DATA_OFFSET]),Data,Length );

	//SPIUART_SendReceive(DataToSend, pResponse);*
	CR95HF_Send_SPI_Command(DataToSend);

// 	// in case of succes CR95HF might not reply
// 	if (pResponse[READERREPLY_STATUSOFFSET] == CR95HF_NOREPLY_CODE)
// 		return CR95HF_SUCCESS_CODE;	
// 			
// 	if (CR95HF_IsReaderResultCodeOk (IDLE,pResponse) != CR95HF_SUCCESS_CODE)
// 		return CR95HF_ERROR_CODE;

	return CR95HF_SUCCESS_CODE;	
}

/**
 *	@brief  this function send a RdReg command to CR95HF
 *  @param  Length 		: Number of bytes
 *  @param	Address		: address of first register to read
 *  @param	RegCount	: number of register to read
 *  @param	Flags		: whether to increment address after register read
 *  @param  pResponse : pointer on CR95HF response
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 *  @return CR95HF_ERROR_CODE : CR95HF returned an error code
 *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : Length parameter is erroneous
 */
int8_t CR95HF_RdReg(uc8 Length,uc8 Address,uc8 RegCount,uc8 Flags,uint8_t *pResponse)
{
uint8_t DataToSend[RDREG_BUFFER_SIZE],
		FlagsMasqued;

	// check the function parameters
	if (Length != 3)
		return CR95HF_ERRORCODE_PARAMETERLENGTH; 

	FlagsMasqued = Flags & 0x03;

	DataToSend[CR95HF_COMMAND_OFFSET ] = READ_REGISTER;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= Length;
	DataToSend[CR95HF_DATA_OFFSET  ]	= Address;
	DataToSend[CR95HF_DATA_OFFSET +1 ]	= RegCount;
	DataToSend[CR95HF_DATA_OFFSET +2 ]	= FlagsMasqued;

	SPIUART_SendReceive(DataToSend, pResponse);
	
	if (CR95HF_IsReaderResultCodeOk (IDLE,pResponse) != CR95HF_SUCCESS_CODE)
		return CR95HF_ERROR_CODE;

	return CR95HF_SUCCESS_CODE;

}

/**
 *	@brief  this function send a WrReg command to CR95HF
 *  @param  Length 		: Number of bytes of WrReg parameters
 *  @param	Address		: address of first register to write
 *  @param	pData 		: pointer data to be write
 *  @param	Flags		: whether to increment address after register read
 *  @param  pResponse : pointer on CR95HF response
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 *  @return CR95HF_ERRORCODE_PARAMETERLENGTH : Length parameter is erroneous
 */
int8_t CR95HF_WrReg (uc8 Length,uc8 Address,uc8 Flags,uc8 *pData,uint8_t *pResponse)
{
uint8_t DataToSend[WRREG_BUFFER_SIZE],
		FlagsMasqued,
		i=0;

	// check the function parameters
	if (Length != 3)
		return ERRORCODE_GENERIC; 

	FlagsMasqued = Flags & 0x03;

	DataToSend[CR95HF_COMMAND_OFFSET ] = WRITE_REGISTER;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= Length;
	DataToSend[CR95HF_DATA_OFFSET  ]	= Address;
	DataToSend[CR95HF_DATA_OFFSET +1 ]	= FlagsMasqued;

	// Parameters[0] first byte to emmit
	for (i =0 ; i< Length - 2 ;i++)
		DataToSend[CR95HF_DATA_OFFSET + 2 + i ] = pData[i];

	SPIUART_SendReceive(DataToSend, pResponse);
	return CR95HF_SUCCESS_CODE;
}

/**
 *	@brief  this function send a BaudRate command to CR95HF
 *  @param	BaudRate	: new baudrate
 *  @param	pResponse 	: pseudo reply (shall be 0x55)
 *  @return CR95HF_SUCCESS_CODE : the command was succedfully send
 */
int8_t CR95HF_BaudRate (uc8 BaudRate,uint8_t *pResponse)
{
uint8_t DataToSend[BAUDRATE_BUFFER_SIZE];

	DataToSend[CR95HF_COMMAND_OFFSET ] = BAUD_RATE;
	DataToSend[CR95HF_LENGTH_OFFSET  ]	= BAUDRATE_LENGTH;
	DataToSend[CR95HF_DATA_OFFSET  ]	= BaudRate;

	SPIUART_SendReceive(DataToSend, pResponse);
	return CR95HF_SUCCESS_CODE;
}

/**
 *	@brief  This function run a process to change UARt baud rate. It'll check the baud rate is
 *	@brief   compatible with this MCU and configurates MCU and CR95HF baud rate.
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @retval  CR95HF_SUCCESS_CODE : the UART baud rate has been succesfuly changed 
 *  @retval  CR95HF_ERRORCODE_UARTDATARATEUNCHANGED : the UART baud rate has not changed 
 *  @retval  CR95HF_ERRORCODE_UARTDATARATEPROCESS : command DaubRate was send to CR95HF but the MCU was not able to comunicatewith CR95HF
 */
int8_t CR95HF_ChangeUARTBaudRate (uc8 *pCommand, uint8_t *pResponse)
{
	*pResponse =CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;

	//if(CR95HF_Interface == SPI)
	if(ReaderConfig.Interface != CR95HF_INTERFACE_UART)
	{	*pResponse =CR95HF_ERRORCODE_UARTDATARATEUNCHANGED;
		return CR95HF_ERRORCODE_UARTDATARATEUNCHANGED; 
	}

	// some UART data rates are not compatible between MCU and CR95HF
	if (CR95HF_IsUartMCUAndCR95HFAvailable(pCommand) != CR95HF_SUCCESS_CODE)
	{	*pResponse =CR95HF_ERRORCODE_UARTDATARATEUNCHANGED;
		return CR95HF_ERRORCODE_UARTDATARATEUNCHANGED; 
	}

	// send baud rate command to CR95HF
	SPIUART_SendReceive(pCommand,pResponse);

	// udpate UART config of MCU
	UART_Initialization(CR95HF_ComputeUARTBaudRate (pCommand[CR95HF_DATA_OFFSET]));

    // check if the MCU can communicate with CR95HF
	CR95HF_Echo(pResponse);
	if (pResponse[ECHOREPLY_OFFSET]!=0x55)
	{	*pResponse =CR95HF_ERRORCODE_UARTDATARATEPROCESS;
		return CR95HF_ERRORCODE_UARTDATARATEPROCESS; 
	}
	// the baud rate has been succesful modified
	*pResponse =CR95HF_SUCCESS_CODE;
	return CR95HF_SUCCESS_CODE; 
}

/**
 *	@brief  This function sends POR sequence. It can be use to initialize CR95HF after a POR.
 *  @param  none
 *  @return CR95HF_ERRORCODE_PORERROR : the POR sequence doesn't succeded
 *  @return CR95HF_SUCCESS_CODE : CR95HF is ready
 */
int8_t CR95HF_PORsequence( void )
{
uint8_t pResponse[10],
		NthAttempt=1;

		CR95HF_Send_IRQIN_NegativePulse();
		delay_ms(5);

do{
		// send an ECHO command and checks CR95HF response 		
		CR95HF_Echo(pResponse);
		if (pResponse[0]==ECHORESPONSE)
			return CR95HF_SUCCESS_CODE;
		

		else if(ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
		{	// send an pulse on IRQ in case of the chip is on sleep state
			if (NthAttempt ==2)
				CR95HF_Send_IRQIN_NegativePulse();
			else if(ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
				CR95HF_Send_SPI_ResetSequence();				
		}
		else NthAttempt = 5;
		delay_ms(50);
	} while (pResponse[0]!=ECHORESPONSE && NthAttempt++ <5);

	return CR95HF_ERRORCODE_PORERROR;
}


/**
  * @}
  */ 


/** @defgroup CR95HF_CustomsFunctions
*  @brief  this set of function send specific functions
 *  @{
 */

/**
 *	@brief  this function sends a custom command to CR95HF device over SPI or UART bus and reveive
 *	@brief  its response.
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @retval none
 */
void CR95HF_SendCustomCommand(uc8 *pCommand, uint8_t *pResponse)
{
	
	*pResponse = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 0x00;
		
	switch (pCommand[CR95HF_COMMAND_OFFSET])
	{
		case CR95HF_RESETSEQUENCE: 		
			CR95HF_Send_SPI_ResetSequence();
		break;
		case CR95HF_PULSE_IRQIN: 
			CR95HF_Send_IRQIN_NegativePulse();
		break;
		case CR95HF_PULSE_SPINSS: 
			CR95HF_Send_SPINSS_NegativePulse();
		break;
		case CR95HF_PULSE_POOLINGREADING: 
			SPIUART_PoolingReading(pResponse);
		break;
		case CR95HF_GETMCUVERSION: 
		 	CR95HF_GetMCUversion(pResponse);
		break;
		
		case CR95HF_GETHARDWAREVERSION: 
		 	CR95HF_GetHardwareVersion(pResponse);
		break;
		
		case CR95HF_GETINTERFACEPINSTATE: 
		 	CR95HF_GetInterfaceBus (pResponse);
		break;
		case CR95HF_SETUSBDISCONNECTSTATE: 
			CR95HF_SetUSBDisconnectPin ( );		
		break;	
		case CR95HF_READCUSTOMTAGMEMORY:
			CR95HF_ReadAllMemory (pCommand,pResponse);
		break;
		case CR95HF_READMCUBUFFER: 
			CR95HF_ReadMCUBuffer(pCommand,pResponse);
		break;
		
		case CR95HF_INVENTORY16SLOTS: 
			CR95HF_RunInventoty16slots (pCommand,pResponse);	
		break;
		case CR95HF_15693ANTICOL: 
			CR95HF_Run15693Anticollision (pCommand,pResponse);	
		break;

#ifdef USE_MSD_DRIVE

		case CR95HF_COPYTAGTOFILE:
			CR95HF_CopyTagToFile ( pCommand, pResponse );
		break;
				case CR95HF_COPYFILETOTAG:
			CR95HF_CopyFileToTag (pCommand, pResponse);
		break;
				
			
#endif
		case CR95HF_POR :
			CR95HF_Por (pCommand, pResponse );
		break;
		case CR95HF_TAG_TRAKING:
			CR95HF_Tag_Traking (pCommand, pResponse );
		break;
		

		
		case CR95HF_GOTOTAGDETECTINGSTATE :
			CR95HF_GotoTagDetectingState (pCommand, pResponse );
		break;
		case CR95HF_ISWAKEUP :
			CR95HF_IsWakeUp (pCommand, pResponse );
		break;
		case CR95HF_CALIBRATETHETAGDETECTION:
			CR95HF_CalibrateTagDetection (pCommand, pResponse );
		break;
		default :
				*pResponse = CR95HF_ERRORCODE_CUSTOMCOMMANDUNKNOWN;
		break;

	}
	
}

/**
 *	@brief  this function returns the inferface bus choosed 
 *  @param  *pResponse : pointer on the CR95HF response
 *  @param  : 0x00 : status ok
 *  @param  : 0x01 : length
 *  @param  : 0x0X : with X = 0 => UART Or X = 0 => SPI Or X = 2 => TWI
 *  @retval 
 */
void CR95HF_GetInterfaceBus (uint8_t *pResponse)
{
   	*pResponse 		= 	SUCCESFUL_COMMAND_0x80;
		*(pResponse+1) 	= 0x01;
		*(pResponse+2) 	= CR95HF_GetInterfacePinState ();
}

/**
 *	@brief  this function returns the inferface bus choosed 
 *  @param  *pCommand : command received 
 *  @retval  none
 */
void CR95HF_SetUSBDisconnectPin ( void )
{
	
//    	if (pCommand[CR95HF_DATA_OFFSET] == DISABLE)
// 		USB_Cable_Config (DISABLE);
// 	else 
// 		USB_Cable_Config (ENABLE);
	#ifdef USE_MSD_DRIVE
		USB_MSD_Reboot();
	#endif 

}


/**
 *	@brief  this function returns the MCU version 
  *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @retval  none
 */
void CR95HF_GetMCUversion(uint8_t *pResponse)
{
	*pResponse = MCUVERSION_STATUS;
	*(pResponse+1) = MCUVERSION_LENGTH;
	memcpy((pResponse+2),MCUVERSION ,MCUVERSION_LENGTH);	
}

/**
 *	@brief  this function returns the MCU version 
  *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @retval  none
 */
void CR95HF_GetHardwareVersion(uint8_t *pResponse)
{
#ifdef USE_MB1054ABOARD 
		// version with the EMI filter
int8_t HardwareVersion []= {'M','B','1','0','5','4','A'},
			NbBytes = 0x07;
#elif USE_MB983_ABOARD  
int8_t HardwareVersion []= {'M','B','9','8','3','_','A'},
			NbBytes = 0x07;
#endif 


	*pResponse = MCUVERSION_STATUS;
	*(pResponse+1) = NbBytes;
	memcpy((pResponse+2),HardwareVersion ,NbBytes);	
}

/**
 * @}
 */


/** @defgroup CR95HF_SPIorUARTFunctions
*  @brief  this set of function manage the communication to CR95HF device. the bus can be either SPI or UART. 
 *  @{
*/


/**
 *	@brief  this function send a command to CR95HF device over SPI or UART bus and reveive its response
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @return CR95HF_SUCCESS_CODE : the function is succesful
 */
int8_t SPIUART_SendReceive(uc8 *pCommand, uint8_t *pResponse)
{

	*pResponse =CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;

	//if(CR95HF_Interface == SPI)
	if(ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
	{
		// First step  - Sending command 
		CR95HF_Send_SPI_Command(pCommand);
		// Second step - Polling
		if (CR95HF_PollingCommand( ) != CR95HF_SUCCESS_CODE)
		{	*pResponse =CR95HF_ERRORCODE_TIMEOUT;
			return CR95HF_POLLING_CR95HF;	
		}
		// Third step  - Receiving bytes 
		CR95HF_Receive_SPI_Response(pResponse);
	}
	else if(ReaderConfig.Interface == CR95HF_INTERFACE_UART)
	{
		// First step  - Sending command
		CR95HF_Send_UART_Command(pCommand);
		// Second step - Receiving bytes 
		CR95HF_Receive_UART_Response(pResponse);
	}
	// update global variable case of shadow mode
	SpyTransaction(pCommand,pResponse);
	return RESULTOK; 
}


/**
 *	@brief  this function is a specific command. It's made polling and reading sequence. 
  *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @return CR95HF_SUCCESS_CODE : the function is succesful
 *  @return CR95HF_POLLING_CR95HF : the polling sequence returns an error
 */
int8_t SPIUART_PoolingReading (uint8_t *pResponse)
{

	*pResponse =CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;

	//if(CR95HF_Interface == SPI)
	if(ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
	{
		// First step - Polling
		if (CR95HF_PollingCommand( ) != CR95HF_SUCCESS_CODE)
		{	*pResponse =CR95HF_ERRORCODE_TIMEOUT;
			return CR95HF_POLLING_CR95HF;	
		}
		// Second step  - Receiving bytes 
		CR95HF_Receive_SPI_Response(pResponse);
	}
	else if(ReaderConfig.Interface == CR95HF_INTERFACE_UART)
	{
		// Second step - Receiving bytes 
		CR95HF_Receive_UART_Response(pResponse);
	}
	return CR95HF_SUCCESS_CODE; 
}



/**
*	 @brief  this function send a command to CR95HF device over SPI or UART bus and receive its response.
*	 @brief  the returned value is CR95HF_SUCCESS_CODE 	
*  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
*  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
*  @return CR95HF_SUCCESS_CODE : the CR95HF returns an success code
*  @return CR95HF_ERROR_CODE : the CR95HF returns an error code
 */
int8_t CR95HF_CheckSendReceive(uc8 *pCommand, uint8_t *pResponse) 
{
	
	SPIUART_SendReceive(pCommand, pResponse);

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) != RESULTOK)
		return CR95HF_ERROR_CODE;

	return RESULTOK;
}


/**
 *	@brief  this function send a command to CR95HF device over SPI or UART bus
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 *  @retval  none
 */
void SPIUART_Send(uc8 *pCommand)
{
	if(ReaderConfig.Interface == CR95HF_INTERFACE_SPI)
		// First step  - Sending command 
		CR95HF_Send_SPI_Command(pCommand);

	else if(ReaderConfig.Interface == CR95HF_INTERFACE_UART)
		// First step  - Sending command
		CR95HF_Send_UART_Command(pCommand);
}

/**
 *	@brief  this functions captures the reader command and updates the global
 *	@brief  variable (case of shadow mode).
 *  @param  *pCommand  : command code
 *  @param  *pResponse : command name
 *  @retval CR95HF_SUCCESS_CODE : the function is succesful
 */
int8_t SpyTransaction(uc8 *pCommand, uint8_t *pResponse)
{
	switch (pCommand[CR95HF_COMMAND_OFFSET])
	{
		case PROTOCOL_SELECT: 
			if (CR95HF_IsReaderResultCodeOk (pCommand[CR95HF_COMMAND_OFFSET],pResponse) == TRUE)
				ReaderConfig.CurrentProtocol=pCommand[CR95HF_DATA_OFFSET];
			return CR95HF_SUCCESS_CODE;	
		default: 
			return CR95HF_SUCCESS_CODE;
	
	}
}

/**
 * @}
 */



/** @defgroup CR95HF_ISFunctions
*  @brief  this set of function checks a parameter and returns either a succesful code or an error code.
 *  @{
 */



/**  
* @brief  this function returns CR95HF_SUCCESS_CODE is the command code exists, CR95HF_ERRORCODE_COMMANDUNKNOWN otherwise
* @param  CmdCode :  	code command send to CR95HF		
* @retval CR95HF_SUCCESS_CODE : the command code is available
* @retval CR95HF_ERRORCODE_COMMANDUNKNOWN : the command code isn't available
*/
int8_t CR95HF_IsCommandExists(uint8_t CmdCode)
{

   CmdCode = CmdCode & 0x0F;
  	switch (CmdCode)
	{
		case ECHO: 
			return CR95HF_SUCCESS_CODE;
		case IDN: 
			return CR95HF_SUCCESS_CODE;
		case PROTOCOL_SELECT: 
			return CR95HF_SUCCESS_CODE;
		case SEND_RECEIVE: 
			return CR95HF_SUCCESS_CODE;
		case IDLE: 
			return CR95HF_SUCCESS_CODE;
		case READ_REGISTER: 
			return CR95HF_SUCCESS_CODE;
		case WRITE_REGISTER: 
			return CR95HF_SUCCESS_CODE;
		case BAUD_RATE: 
			return CR95HF_SUCCESS_CODE;
		default: 
			return CR95HF_ERRORCODE_COMMANDUNKNOWN;
	}
}


/**  
* @brief  	this function returns CR95HF_SUCCESS_CODE is the reader reply is a succesful code.
* @param  	CmdCode		:  	code command send to the reader
* @param  	ReaderReply	:  	pointer on CR95HF response	
* @retval  	CR95HF_SUCCESS_CODE :  CR95HF returned a succesful code
* @retval  	CR95HF_ERROR_CODE  :  CR95HF didn't return a succesful code
* @retval  	CR95HF_NOREPLY_CODE : no CR95HF response
*/
int8_t CR95HF_IsReaderResultCodeOk (uint8_t CmdCode,uc8 *ReaderReply)
{

   CmdCode = CmdCode & 0xFF;

   if (ReaderReply[READERREPLY_STATUSOFFSET] == CR95HF_ERRORCODE_DEFAULT)
		return CR95HF_NOREPLY_CODE;

  	switch (CmdCode)
	{
		case ECHO: 
			if (ReaderReply[PSEUDOREPLY_OFFSET] == ECHO)
				return CR95HF_SUCCESS_CODE;
			else 
				return CR95HF_ERROR_CODE;
		case IDN: 
			if (ReaderReply[READERREPLY_STATUSOFFSET] == IDN_RESULTSCODE_OK)
				return CR95HF_SUCCESS_CODE;
			else 
				return CR95HF_ERROR_CODE;
		case PROTOCOL_SELECT: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case IDN_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				case PROTOCOLSELECT_ERRORCODE_CMDLENGTH :
					return CR95HF_ERROR_CODE;
				case PROTOCOLSELECT_ERRORCODE_INVALID :
					return CR95HF_ERROR_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case SEND_RECEIVE: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case SENDRECV_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_COMERROR :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_FRAMEWAIT :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_SOF :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_OVERFLOW :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_FRAMING :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_EGT :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_LENGTH :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_CRC :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_RECEPTIONLOST :
					return CR95HF_ERROR_CODE;
				default :
					return CR95HF_ERROR_CODE;
			}			
		case IDLE: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case IDLE_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				case IDLE_ERRORCODE_LENGTH :
					return CR95HF_ERROR_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case READ_REGISTER: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case READREG_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				case READREG_ERRORCODE_LENGTH :
					return CR95HF_ERROR_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case WRITE_REGISTER: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case WRITEREG_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case BAUD_RATE: 
			return CR95HF_ERROR_CODE;
		default: 
			return ERRORCODE_GENERIC;
	}
}

/**  
* @brief  	this function returns CR95HF_SUCCESS_CODE is the reader reply is an error code.
* @param  	CmdCode		:  	code command send to the reader
* @param  	ReaderReply	:  	pointer on CR95HF response	
* @retval  	CR95HF_SUCCESS_CODE :  CR95HF returned a error code
* @retval  	CR95HF_ERROR_CODE  :  CR95HF didn't return a error code
*/
int8_t CR95HF_IsReaderErrorCode (uint8_t CmdCode,uint8_t *ReaderReply)
{
   CmdCode = CmdCode & 0xFF;
  	switch (CmdCode)
	{
		case ECHO: 
			if (ReaderReply[PSEUDOREPLY_OFFSET] == ECHO)
				return CR95HF_ERROR_CODE;
			else 
				return CR95HF_SUCCESS_CODE;
		case IDN: 
			if (ReaderReply[READERREPLY_STATUSOFFSET] == IDN_RESULTSCODE_OK)
				return CR95HF_ERROR_CODE;
			else 
				return CR95HF_ERROR_CODE;
		case PROTOCOL_SELECT: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case IDN_RESULTSCODE_OK :
					return CR95HF_ERROR_CODE;
				case PROTOCOLSELECT_ERRORCODE_CMDLENGTH :
					return CR95HF_SUCCESS_CODE;
				case PROTOCOLSELECT_ERRORCODE_INVALID :
					return CR95HF_SUCCESS_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case SEND_RECEIVE: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case SENDRECV_RESULTSCODE_OK :
					return CR95HF_ERROR_CODE;
				case SENDRECV_ERRORCODE_COMERROR :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_FRAMEWAIT :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_SOF :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_OVERFLOW :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_FRAMING :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_EGT :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_LENGTH :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_CRC :
					return CR95HF_SUCCESS_CODE;
				case SENDRECV_ERRORCODE_RECEPTIONLOST :
					return CR95HF_SUCCESS_CODE;
				default :
					return CR95HF_ERROR_CODE;
			}			
		case IDLE: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case IDLE_RESULTSCODE_OK :
					return CR95HF_ERROR_CODE;
				case IDLE_ERRORCODE_LENGTH :
					return CR95HF_SUCCESS_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case READ_REGISTER: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case READREG_RESULTSCODE_OK :
					return CR95HF_ERROR_CODE;
				case READREG_ERRORCODE_LENGTH :
					return CR95HF_SUCCESS_CODE;
				default : return CR95HF_ERROR_CODE;
			}
		case WRITE_REGISTER: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{
				case WRITEREG_RESULTSCODE_OK :
					return CR95HF_SUCCESS_CODE;
				default : return CR95HF_SUCCESS_CODE;
			}
		case BAUD_RATE: 
			return CR95HF_SUCCESS_CODE;
		case SUB_FREQ_RES: 
			switch (ReaderReply[READERREPLY_STATUSOFFSET])
			{	case SUBFREQ_RESULTSCODE_OK :
					return CR95HF_ERROR_CODE;
				default : return CR95HF_ERROR_CODE;
			}	

		default: 
			return ERRORCODE_GENERIC;
	}
}

  /**
 * @}
 */


/** @defgroup CR95HF_SplitFunctions
 *  @{
 */


/**  
* @brief    this function split a CR95HF response and extract the different fields
* @param  	CmdCodeToReader : code command send to the reader
* @param	ProtocolSelected : protocol selected (select command)
* @param 	ReaderReply 	: reader reply
* @param 	ResultCode		: TRUE is reponse is Ok, false otherwise 
* @param 	NbTagByte		: Number of byte of tag reply
* @param 	TagReplyDataIndex : data of the reader reply is exists
* @param 	NbControlByte	: Number of control byte
* @param 	ControlIndex	: control byte(s)
* @retval 	CR95HF_SUCCESS_CODE : CR95HF was succesful splitted
* @retval 	CR95HF_ERRORCODE_COMMANDUNKNOWN : The command code is unknown
*/
int8_t SplitReaderReply (uint8_t CmdCodeToReader,u8 ProtocolSelected,uc8 *ReaderReply,uint8_t *ResultCode,uint8_t *NbTagByte,uint8_t *TagReplyDataIndex,uint8_t *NbControlByte,uint8_t *ControlIndex)
{	
	// output parameters initialization
	*NbTagByte= 0;
	*ResultCode = FALSE;

	if (  CR95HF_IsCommandExists(CmdCodeToReader) != CR95HF_SUCCESS_CODE)
		return CR95HF_ERRORCODE_COMMANDUNKNOWN;
	
	*ResultCode = CR95HF_SUCCESS_CODE;

	// the ECHO and Baud rate commands reply with a pseudo reponse (0x55)
	if ((CmdCodeToReader == ECHO) || (CmdCodeToReader == BAUD_RATE))
	{
		//memcpy(TagReplyData,&ReaderReply[PSEUDOREPLY_OFFSET],PSEUDOREPLY_LENGTH);
		*TagReplyDataIndex = PSEUDOREPLY_OFFSET;
		*NbTagByte = PSEUDOREPLY_LENGTH;
	}
	else if (CR95HF_IsReaderResultCodeOk (CmdCodeToReader,ReaderReply)== CR95HF_SUCCESS_CODE)
	{
		// reply 00 00 (Length = 0)
		if ( ( ReaderReply[READERREPLY_STATUSOFFSET] & 0x80 == 0) &&
			 (ReaderReply[READERREPLY_STATUSOFFSET+1] == 0x00))
		{
			//memcpy(TagReplyData,&ReaderReply[CR95HF_DATA_OFFSET-1],1);
			*TagReplyDataIndex = CR95HF_DATA_OFFSET;
			*NbTagByte = 1;
		}
		// case (Length > 0)
		else		
		{
			*NbControlByte = GetNbControlByte (ProtocolSelected);
			*TagReplyDataIndex = CR95HF_DATA_OFFSET;
			*ControlIndex = CR95HF_DATA_OFFSET+*NbTagByte;
			*NbTagByte = ReaderReply[CR95HF_LENGTH_OFFSET]-*NbControlByte;
		}
	}
	// Error Code
	else
	{
	 	//memcpy(TagReplyData,&ReaderReply[CR95HF_DATA_OFFSET-2],ERRORCODE_LENGTH);
		*TagReplyDataIndex = CR95HF_DATA_OFFSET-2;
		*NbTagByte = ERRORCODE_LENGTH;
		*ResultCode = CR95HF_ERROR_CODE;
	}
	
	return CR95HF_SUCCESS_CODE;
}




/**  
 * @brief  this function reads the TAG memory and save it on STM32 memory
 * @brief  with CRC control.
 * @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 * @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 * @retval CR95HF_SUCCESS_CODE
 */
static int8_t CR95HF_ReadAllMemory (uc8 *pCommand, uint8_t *pResponse )
{
uint8_t /*ReadBuffer [6]={0x04, 0x03, 0x02, 0x20},*/
			//	RepBuffer[32],
//				ReadBufGetInf[4] = {0x04, 0x02, 0x02, 0x2B},
				IC_Ref_Tag,
				RequestFlags = 0x02,
				Tag_Density;
int8_t 	status; 
uint16_t NbBlockToRead = 0, 
				AddStart =0,
				NbMemoryBlock,
				TotalCRCError = 0,
				NbCRCError = 0,
				OffsetBuffer = 0,
				NthDataToRead = 0;


	// the received command is composed as B0 03 XX XX YY with 
	// B0 : operating command code
	// XX XX : first block address
	// YY : number of block

	*pResponse =CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;

	/*Send Protocol Select ISO/IEC 15693 protocol and identify the ISO/IEC 15693 tag*/
	errchk(ISO15693_GetTagIdentification(&NbMemoryBlock,&Tag_Density, &IC_Ref_Tag));	

	/*Read the number of adress byte*/
	/*Low density memory : the number of byte of the memory address is one */
	if (pCommand [CR95HF_DATA_OFFSET+2] == 0x00)
			NbBlockToRead = NbMemoryBlock;
	else 
	{
			AddStart = ((pCommand[CR95HF_DATA_OFFSET]<<8) & 0xFF00) | pCommand[CR95HF_DATA_OFFSET+1] ;
			NbBlockToRead = pCommand [4]+1;
	}
	
	// for the high density tag, the read command shall be updated
	if (Tag_Density == ISO15693_HIGH_DENSITY)
	{
// 		/*Parameters for two adress bits*/
// 		//Length SendRec
// 		ReadBuffer[1] = 0x04;
// 			//copies the Request flags
// 		ReadBuffer[2] = 0x0A;
			RequestFlags = 0x0A;
	}
		
	for ( NthDataToRead=AddStart; NthDataToRead<(NbBlockToRead+AddStart); NthDataToRead++)
	{						
// 		if (pCommand[1]==0x02)
// 			ReadBuffer [4] = (NthDataToRead) & 0x00FF;
// 		else if (pCommand[1]==0x03)
// 		{
// 			ReadBuffer [4] = (NthDataToRead) & 0x00FF;
// 			ReadBuffer [5] = ((NthDataToRead) & 0xFF00) >>8;
// 		}

		/*Read the data from the conctacless tag*/ 
		do
		{
			//SPIUART_SendReceive(ReadBuffer, RepBuffer);
			status = ISO15693_ReadSingleBlock 	(RequestFlags, 0x00, NthDataToRead,ReaderRecBuf );
			NbCRCError++;
		}while (status != RESULTOK && NbCRCError < 3);

		/*CRC Error Compter*/
		if((ReaderRecBuf[9] & 0x03) != 0x00)
			TotalCRCError++;

		memcpy(&(TagMemoryBuffer[(OffsetBuffer)*4]),&(ReaderRecBuf[3]),4);

		NbCRCError=0;
		OffsetBuffer++;

	}

	/*Memory Read Parameters*/	
	Glo_NbHIDTrame = (NbBlockToRead/0x0F)+1;

	/*Error code if more than 25% error during reading*/
	if(TotalCRCError > (NbBlockToRead/4))
	{
		*(pResponse) = CR95HF_ERRORCODE_READALLMEMORY;
		return CR95HF_SUCCESS_CODE;
	}
	else 
		*(pResponse) = SUCCESFUL_COMMAND_0x80;
	
		*(pResponse+1) = 0x02;
		*(pResponse+2) = Glo_NbHIDTrame;
		*(pResponse+3) = TotalCRCError;
		

	return CR95HF_SUCCESS_CODE;
Error : 
	*(pResponse) = CR95HF_ERRORCODE_READALLMEMORY;
	return CR95HF_SUCCESS_CODE;		
}

/**  
 * @brief  this function sends 15 blocks of TAG memory by USB HID 
 * @brief  the Tag memery need to be store into STM32 memory before use 
 * @brief  this function. 
 * @param  none
 * @retval CR95HF_SUCCESS_CODE
 */
//static void CR95HF_Send_Tram (uc8 *pCommand, uint8_t *pResponse )
//static void CR95HF_ReadMCUBuffer (uc8 *pCommand, uint8_t *pResponse )
//{

//uint8_t	  NumTram = MIN((Glo_NbHIDTrame-1),pCommand[CR95HF_DATA_OFFSET]);
//	
//			/*Copy Data of Select Bloc for USB Transfert*/
//	 		*(pResponse) = SUCCESFUL_COMMAND_0x80;
//			*(pResponse+1) = 0x3D;
//			*(pResponse+2) = NumTram;
//			 memcpy(&pResponse[3],&TagMemoryBuffer[NumTram*0x3C],0x3C);
//}

static void CR95HF_ReadMCUBuffer (uc8 *pCommand, uint8_t *pResponse )
{
	memcpy(pResponse,TagMemoryBuffer,TagMemoryBuffer[1]+2);
	*(TagMemoryBuffer) = SUCCESFUL_COMMAND_0x80;
	*(TagMemoryBuffer+1) = 0;
}




/**  
 * @brief  this function runs the 15693 anticollision process.
 * @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 * @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 * @retval CR95HF_SUCCESS_CODE
 * @retval ERRORCODE_GENERIC : anticollision doesn't succeed
 */

int8_t CR95HF_Run15693Anticollision (uc8 *pCommand, uint8_t *pResponse )
{
uint8_t 	Flags ,
			AFI = 0x00,
			NbTag = 0x00,
			pUIDout [20*9]; // to save up to 20 UID
int8_t		status;
	
	// initialize the USB response
	*pResponse = CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;


	// the received command is composed as A0 XX YY with 
	// A0 : operating command code
	// XX : number of byte
	// YY : request flags

	// the response is composed as SC NB NT XX... with 
	// SC : status code (0x00 for a success exucution)
	// NB : number of byte
	// NT : number of tags seen
	// XX : UID of the tags seen
	
	// get the request flags	
	Flags= pCommand[CR95HF_DATA_OFFSET]; 
	
	errchk(ISO15693_RunAntiCollision (Flags , AFI,&NbTag,pUIDout));

	// create the response
	*pResponse = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 1+(ISO15693_NBBYTE_UID+1)*NbTag;
	*(pResponse+2) = NbTag;
	if (NbTag > 0 && NbTag < 20) 
		memcpy((pResponse+3),pUIDout, (ISO15693_NBBYTE_UID+1)*NbTag); 


   	return CR95HF_SUCCESS_CODE;
Error:
	return ERRORCODE_GENERIC;
}

/**  
 * @brief  this function runs the 15693 anticollision process.
 *  @param  *pCommand  : pointer on the buffer to send to the CR95HF ( Command | Length | Data)
 *  @param  *pResponse : pointer on the CR95HF response ( Command | Length | Data)
 * @retval CR95HF_SUCCESS_CODE
 * @retval ERRORCODE_GENERIC : anticollision doesn't succeed
 */

int8_t CR95HF_RunInventoty16slots (uc8 *pCommand, uint8_t *pResponse )
{
uint8_t 	Flags ,
					AFI = 0x00,
					NbTag = 0x00,
					pUIDout [20*9]; // to save up to 20 UID
int8_t		status;
	
	// initialize the USB response
	*pResponse = CR95HF_ERRORCODE_DEFAULT;
	*(pResponse+1) = 0x00;


	// the received command is composed as A0 XX YY with 
	// A1 : operating command code
	// XX : number of byte
	// YY : request flags

	// the response is composed as SC NB NT XX... with 
	// SC : status code (0x00 for a success exucution)
	// NB : number of byte
	// NT : number of tags seen
	// XX : UID of the tags seen
	
	// get the request flags	
	Flags= pCommand[CR95HF_DATA_OFFSET]; 
	
	errchk(ISO15693_RunInventory16slots (Flags , AFI,&NbTag,pUIDout));

	// create the response
	*pResponse = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 1+8*NbTag;
	*(pResponse+2) = NbTag;
	if (NbTag > 0 && NbTag < 16) 
		memcpy((pResponse+3),pUIDout, 8*NbTag); 


   	return CR95HF_SUCCESS_CODE;
Error:
	return ERRORCODE_GENERIC;
}



/**  
 * @brief  this function loads the data from a file in the mass storage device 
 * @brief  and write them in the TAG Memory, this fonction can write only 32kbits 
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE.
	C4: Command name
	00: Length Data

 */
static int8_t CR95HF_Tag_Traking (uc8 *pCommand, uint8_t *pResponse )
{

	HID_TRANSACTION = FALSE;
 	*(pResponse) = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 0x00;

	return CR95HF_SUCCESS_CODE;
}



/**  
 * @brief  this function loads the data from a file in the mass storage device 
 * @brief  and write them in the TAG Memory, this fonction can write only 32kbits 
 * @param  none
 * @retval CR95HF_SUCCESS_CODE.
	C3: Command name
	00: Length Data

 */
static int8_t CR95HF_Por (uc8 *pCommand, uint8_t *pResponse )
{

	if(CR95HF_PORsequence() == CR95HF_SUCCESS_CODE)
	{
	*(pResponse) = 0x80;
	*(pResponse+1) = 0x00;
	}
	else
	{
	*(pResponse) = 0x87;
	*(pResponse+1) = 0x00;
	}



	return CR95HF_SUCCESS_CODE;
}



/**  
 * @brief  this function launches the POR sequence?
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE
 */
static int8_t CR95HF_GotoTagDetectingState	( uc8 *pCommand, uint8_t *pResponse )
{
uint8_t IdleData[]	= {0x0A,0x21,0x00,0x79,0x01,0x18,0x00,0x20,0x60,0x60,0x6C,0x74,0x3F,0x01};

 	*(pResponse) = CR95HF_ERRORCODE_TAGDETECTING;
	*(pResponse+1) = 0x00;

	// the received command is composed as A3 02 XX YY 
	// A3 : operating command code
	// 02 : number of byte
	// XX : DACdataL
	// YY : DACdataH

	// reset the CR95HF
	CR95HF_Send_SPI_ResetSequence();		
	CR95HF_Echo(ReaderRecBuf);
	if (ReaderRecBuf[0] != ECHORESPONSE)
			return CR95HF_ERRORCODE_TAGDETECTING;
	
	IdleData [0] = CR95HF_WAKEUPSOURCE_TAGDETECTING | CR95HF_WAKEUPSOURCE_IRRQINN;
	// copies the DACdataL & DACdataH values 
	IdleData [10] = pCommand[CR95HF_DATA_OFFSET];
	IdleData [11] = pCommand[CR95HF_DATA_OFFSET+1];
	CR95HF_Idle(0x0E,IdleData);
	
   	// enable the interuption on the IRQout to catch the CR95HF response
	// -- An EXTI must be set to catch the IRQ (a edge on the UART RX pin) from the CR95HF
	//EXTI_Config( );	  
	// enable the interupt 
//	IRQOUT_Interrupts_Config (); 
//	CR95HF_DataReady == FALSE; 
	ReaderConfig.State = CR95HF_STATE_TAGDETECTOR;
	*(pResponse) = SUCCESFUL_COMMAND_0x80;
	return CR95HF_SUCCESS_CODE;	
	

}


/**  
 * @brief  this function sends to the CR95HF a Idle command.
 * @brief  The wake up sources are IRQ_in and tag detecting. 
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE : the function is succesful
 * @retval CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION : the function is not succesful
 */
static int8_t CR95HF_GetTagDetectionRefValue	( uint8_t *DacDataRef )
{

uint8_t i=0,
				IdleData[]={0x03,0xA1,0x00,0xF8,0x01,0x18,0x00,0x20,0x60,0x60,0x00,0x00,0x3F,0x01};
								
					
	(*DacDataRef )	= 0xFC;

	// reset the CR95HF
	CR95HF_Send_SPI_ResetSequence();		
	CR95HF_Echo(ReaderRecBuf);
	if (ReaderRecBuf[0] != ECHORESPONSE)
			return CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION;
	
	
	// the received command is composed as A4 00 00 
	// A4 : operating command code
	// 00 : number of byte
		
	// 1st step : VERIFY detection with DacDataH=0x00 (min value)
	//  070E03A100F801180020606000003F01
	CR95HF_Idle(0x0E,IdleData);
	SPIUART_PoolingReading (ReaderRecBuf);
	if (CR95HF_IsWakeUpByTagDetecting(ReaderRecBuf) != TRUE)
			return CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION; 
	
	//2nd step : VERIFY no detection with DacDataH=0xFC (max value)
	// 070E03A100F801180020606000FC3F01
	IdleData [11]= (*DacDataRef);  
	CR95HF_Idle(0x0E,IdleData);
	SPIUART_PoolingReading (ReaderRecBuf);
	if (CR95HF_IsWakeUpByTimeOut(ReaderRecBuf) != TRUE)
			return CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION; 
	
	// next steps : Calibration with DacDataH=0x1F
	for (i=0;i<=5;i++)
	{
		
			if (CR95HF_IsWakeUpByTimeOut(ReaderRecBuf) == TRUE)
					(*DacDataRef )-= (0x080 >> i);
			else if (CR95HF_IsWakeUpByTagDetecting(ReaderRecBuf) == TRUE)
				(*DacDataRef ) += (0x080 >> i) ;
			else return CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION;
			
			IdleData [11]=(*DacDataRef); 
			CR95HF_Idle(0x0E,IdleData);
			SPIUART_PoolingReading (ReaderRecBuf);
	}
	
	if (CR95HF_IsWakeUpByTimeOut(ReaderRecBuf) == TRUE)
		*DacDataRef -= 4; 
	else if (CR95HF_IsWakeUpByTagDetecting(ReaderRecBuf) == TRUE)
		(*DacDataRef) = (*DacDataRef); 
	
	return CR95HF_SUCCESS_CODE;	
}

/**  
 * @brief  this function sends to the CR95HF a Idle command.
 * @brief  The wake up sources are IRQ_in and tag detecting. 
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE : the function is succesful
 * @retval CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION : the function is not succesful
 */
int8_t CR95HF_CalibrateTagDetection	(uc8 *pCommand, uint8_t *pResponse )
{
uint8_t DacDataRef;
	
	*(pResponse) = CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION;
	*(pResponse+1) = 00;	
	
		if ( CR95HF_GetTagDetectionRefValue(&DacDataRef) == CR95HF_SUCCESS_CODE)
		{	
				// creates the response
			*(pResponse) = SUCCESFUL_COMMAND_0x80;
			*(pResponse+1) = 0x02;
			*(pResponse+2) = DacDataRef - 0x08;
			*(pResponse+3) = DacDataRef + 0x08;
		}

		return CR95HF_SUCCESS_CODE;	
	
}


/**  
 * @brief  this function returns TRUE if the wake up source is the internal timeout
 * @param  IdleResponse	: reponse of the CR95HF to the Idle command
 * @retval TRUE : the timeout of the CR95Hf is the source of the wake up
 * @retval FALSE : the timeout of the CR95Hf is not the source of the wake up
 */
static int8_t CR95HF_IsWakeUpByTimeOut	(uint8_t *IdleResponse)
{
		if (IdleResponse[0] == CR95HF_SUCCESS_CODE &&
				IdleResponse[1] == CR95HF_IDLERES_LENGTH &&
				IdleResponse[2] == CR95HF_WAKEUPSOURCE_TIMEOUT )
		
				return TRUE ;
		else
			return FALSE ;
}

/**  
 * @brief  this function returns TRUE if the wake up source is the tag detecting block
 * @param  IdleResponse	: reponse of the CR95HF to the Idle command
 * @retval TRUE : the tag detecting of the CR95Hf is the source of the wake up
 * @retval FALSE : the tag detecting of the CR95Hf is not the source of the wake up
 */
static int8_t CR95HF_IsWakeUpByTagDetecting	(uint8_t *IdleResponse)
{
		if (IdleResponse[0] == CR95HF_SUCCESS_CODE &&
				IdleResponse[1] == CR95HF_IDLERES_LENGTH &&
				IdleResponse[2] == CR95HF_WAKEUPSOURCE_TAGDETECTING )
		
				return TRUE ;
		else
			return FALSE ;
}


/**  
 * @brief  this function launches the POR sequence?
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE
 */
static int8_t CR95HF_IsWakeUp	(uc8 *pCommand, uint8_t *pResponse )
{
 	*(pResponse) = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 0x01;
	// the received command is composed as A2 00 
	// A2 : operating command code
	// 00 : number of byte
	
	if ( GetIRQOutState () != 0x00)
		*(pResponse+2) = 0x01;
	else 
		*(pResponse+2) = 0x00;
				
	return CR95HF_SUCCESS_CODE;	
	

}

/**  
 * @brief  this function launches the POR sequence?
 * @param  pCommand	: HID commend received from the PC
 * @param  pResponse : HID response to be send to the PC
 * @retval CR95HF_SUCCESS_CODE
 */
int8_t GetIRQOutState ( void )
{
	if ((GPIO_ReadInputData(EXTI_GPIO_PORT) & 0x0008) != 0x00)
		return 0x01;
	else 
		return 0x00;
}


#ifdef USE_POOL_FIELD
/**  
 * @brief  this function pool for field around the CR95HF?
 * @param  Pool_Field_States : pDataRead[2] = 1 if a field is detetected, pDataRead[2] = 0 in other case
 * @retval CR95HF_SUCCESS_CODE
 */
int8_t CR95HF_Pool_Field (uint8_t *pDataRead)
{

uint8_t Pool_Field[] = {0x03, 0x00};


if(CR95HF_CheckSendReceive(Pool_Field, pDataRead)!= RESULTOK)
	{
		return ERRORCODE_GENERIC;
	}

	if(pDataRead[CR95HF_DATA_OFFSET] == 0x01)
	{
	/*Implement The CR95HF routine for TAG EMULATION*/
	}

	else
	{
	}


	return CR95HF_SUCCESS_CODE;
}

#endif /*USE POOL_FIELD*/
/**
 * @}
 */



#ifdef USE_MSD_DRIVE
/**
* @brief  Read the TAG Memory and write the data in the file "READ_TAG.txt"
* @param  *pCommand : Command for the function
* @param  *pResponse : return the response to send at the user by USB HID
* @retval CR95HF_SUCCESS_CODE.
	C1: Command name
	04: Length Data
	XX: MSB Nb Bytes to Read 
	XX: LSB Nb Bytes to Read
	XX: MSB First Byte to Read
	XX: LSB First Byte to Read
*/
static uint8_t CR95HF_CopyTagToFile(uc8 *pCommand, uint8_t *pResponse)
{
uint16_t 	NbMemoryBlock = 0x00,
					NbBytes_To_Read,
					FirstBytes_To_Read;
int8_t  	status;
uint8_t  	Tag_Density,
					IC_Ref_Tag;
char    	 File_Name[] = {"0:READ_TAG.bin"};

		
		FirstBytes_To_Read = pCommand[2] <<8 | pCommand[3];
		NbBytes_To_Read = pCommand[4] <<8 | pCommand[5];

		// Configure the CR95HF as ISO15693 reader 
		ISO15693_SelectProtocol();
		// get the identification of the tag 
		errchk(ISO15693_GetTagIdentification(&NbMemoryBlock,&Tag_Density, &IC_Ref_Tag));

		if (NbBytes_To_Read == 0x0000)
			NbBytes_To_Read = NbMemoryBlock * 0x04;
	
		
		ISO15693_Read_Bytes_Tag_Data(Tag_Density, IC_Ref_Tag, TagMemoryBuffer, NbBytes_To_Read, FirstBytes_To_Read);

		Write_MSD_File(TagMemoryBuffer, NbBytes_To_Read, File_Name);

		/*Rebbot the USB connection to force windows to update the FAT table of the MSD*/
		USB_MSD_Reboot();

		*(pResponse) = SUCCESFUL_COMMAND_0x80;
		*(pResponse+1) = 0x00;
	
	return CR95HF_SUCCESS_CODE;
		
Error : 
		pResponse [0] = CR95HF_ERRORCODE_NOTAGFOUND;
		pResponse [1] = 0x00;
		return CR95HF_ERRORCODE_NOTAGFOUND;
}

/**
* @brief  Read the data from the file "WRIT_TAG.txt" and write them in the TAG
* @param  *pCommand : Command for the function
* @param  *pResponse : return the response to send at the user by USB HID
* @retval CR95HF_SUCCESS_CODE.
	C2: Command name
	04: Length Data
	XX: MSB Nb Bytes to Write
	XX: LSB Nb Bytes to Write
	XX: MSB First Byte to Write
	XX: LSB First Byte to Write

*/
static uint8_t CR95HF_CopyFileToTag (uc8 *pCommand, uint8_t *pResponse)
{
uint16_t Length_Memory_TAG,
				 NbBytes_To_Write_Cmd,
				 NbBytes_To_Write,
				 FirstBytes_To_Write;
uint8_t  Tag_Density,
				 IC_Ref_Tag;

char     File_Name[] = {"0:WRIT_TAG.bin"};

	FirstBytes_To_Write = pCommand[2] <<8 | pCommand[3];
	NbBytes_To_Write_Cmd = pCommand[4] <<8 | pCommand[5];
	
	
	
	Read_MSD_File(&TagMemoryBuffer[0], &NbBytes_To_Write, File_Name);

	ISO15693_SelectProtocol();
	ISO15693_GetTagIdentification(&Length_Memory_TAG,&Tag_Density, &IC_Ref_Tag);

	if(NbBytes_To_Write_Cmd == 0x0000)
		ISO15693_Write_Bytes_Tag_Data(Tag_Density, TagMemoryBuffer, NbBytes_To_Write, FirstBytes_To_Write);	
	else
		ISO15693_Write_Bytes_Tag_Data(Tag_Density, TagMemoryBuffer, NbBytes_To_Write_Cmd, FirstBytes_To_Write);
	
	/*Reboot USB conection if the writing time is near 20 secondes*/
	if(IC_Ref_Tag == ISO15693_LRiS64K && NbBytes_To_Write_Cmd == 0x0000)
		USB_MSD_Reboot();
	
	*(pResponse) = SUCCESFUL_COMMAND_0x80;
	*(pResponse+1) = 0x00;
	
	
		return CR95HF_SUCCESS_CODE;
}


/**
* @brief  Read a file in the Mass Storage Device
* @param  *File_Data : return the data read in the file
* @param  *Length : return the length of the data read
* @param  *File_Name : Define the name of the file to read
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	
uint8_t Read_MSD_File(uint8_t *File_Data, uint16_t *Length, const char *File_Name)
	{
	
	uint16_t Size_Read;

		f_mount(0, &ff);
		/*Read the file if is present on the MSD, else return error statement*/
  	res = f_open(&fdft, File_Name, FA_OPEN_EXISTING | FA_READ);

			if(res != FR_OK)
			return ERRORCODE_GENERIC;
		
	Size_Read = f_size(&fdft);

	*Length = Size_Read;
			
	res = f_read(&fdft, File_Data, Size_Read, &br);
		
		f_close(&fdft);
	  f_mount(0, NULL);
		
		return RESULTOK;
}

/**
* @brief  Write a file in the Mass Storage Device
* @param  *File_Data : Data to write in the file
* @param  *Length : Length of the data to write
* @param  *File_Name : Define the name of the file to write
* @retval ERRORCODE_GENERIC / RESULTOK.
*/		
uint8_t Write_MSD_File(uint8_t *File_Data, uint16_t Length, const char *File_Name)
{

  f_mount(0, &ff);
	/*Create a new file if the file name is not present, else overwrite the file*/ 
	res = f_open(&fdft, File_Name, FA_OPEN_ALWAYS | FA_WRITE);

	  	if(res != FR_OK)
			return ERRORCODE_GENERIC;
			
	res = f_write(&fdft, File_Data ,Length , &bw);

			if(res != FR_OK)
			return ERRORCODE_GENERIC;
			
	 f_close(&fdft);
	 f_mount(0, NULL);

return RESULTOK;
}	


/**
* @brief  Reboot the USB Connection
* @param  VOID
* @retval RESULTOK.
*/	
uint8_t USB_MSD_Reboot(void)
{
		/*Use the PIN USB Disconnect to stop the data exchange in the USB Bus*/
		GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
		delayHighPriority_ms(1000);
		GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
		delayHighPriority_ms(1000);		

		return RESULTOK;
}	

#endif


/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/

