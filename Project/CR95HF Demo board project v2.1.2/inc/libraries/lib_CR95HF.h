/**
  ******************************************************************************
  * @file    lib_CR95HF.h 
  * @author  MMY Application Team
  * @version V1.3
  * @date    30/03/2011
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LIB_CR95HF_H
#define _LIB_CR95HF_H


#define USE_POLL_FIELD
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "drv_CR95HF.h"


/* CR95HF HEADER command definition ---------------------------------------------- */
#define CR95HF_COMMAND_SEND													0x00
#define CR95HF_COMMAND_RESET												0x01
#define CR95HF_COMMAND_RECEIVE											0x02
#define CR95HF_COMMAND_POLLING											0x03

/* CR95HF command definition */
#define IDN																					0x01
#define PROTOCOL_SELECT 														0x02
#define POLL_FIELD 																	0x03
#define SEND_RECEIVE																0x04
#define LISTEN																			0x05
#define SEND																				0x06
#define IDLE																				0x07
#define READ_REGISTER																0x08
#define WRITE_REGISTER															0x09
#define BAUD_RATE																		0x0A
#define SUB_FREQ_RES																0x0B
#define AC_FILTER																		0x0D
#define TEST_MODE																		0x0E
#define SLEEP_MODE																	0x0F
#define ECHO																				0x55
// customs command

#define CR95HF_15693ANTICOL 												0xA0
#define CR95HF_INVENTORY16SLOTS											0xA1
#define CR95HF_ISWAKEUP															0xA2
#define CR95HF_GOTOTAGDETECTINGSTATE								0xA3
#define CR95HF_CALIBRATETHETAGDETECTION							0xA4

#define CR95HF_READCUSTOMTAGMEMORY									0xB0
#define CR95HF_READMCUBUFFER												0xB1
#define CR95HF_GETHARDWAREVERSION										0xB2
#define CR95HF_DOWNLOADSTM32MEM											0xB5
#define CR95HF_READWHOLEMEMORY											0xB6
#define CR95HF_TRANSPARENT													0xB7
#define CR95HF_PULSE_POOLINGREADING	   					   	0xB8
#define CR95HF_PULSE_SPINSS			     					  		0xB9
#define CR95HF_GETINTERFACEPINSTATE   					   	0xBA
#define CR95HF_SETUSBDISCONNECTSTATE   					  	0xBB
#define CR95HF_GETMCUVERSION												0xBC
#define CR95HF_RESETSEQUENCE												0xBD
#define CR95HF_PULSE_IRQIN													0xBE


#ifdef USE_MSD_DRIVE
	
#define CR95HF_COPYTAGTOFILE												0xC1
#define CR95HF_COPYFILETOTAG												0xC2

#endif /*USE_MSD_DRIVE*/

#define CR95HF_POR																	0xC3
#define CR95HF_TAG_TRAKING													0xC4
#define CR95HF_CRYPTO_GENERIC												0xC5
#define CR95HF_NDEF_MESSAGE													0xC6




#define SUCCESFUL_SENDS															0x80
#define SUCCESFUL_COMMAND														0x00
#define SUCCESFUL_COMMAND_0x80											0x80
//#define COMMAND_FAILED	

// CR95HF status
#define CR95HF_SUCCESS_CODE													0x00
#define CR95HF_NOREPLY_CODE													0x01
	
// CR95HF polling status
#define CR95HF_POLLING_CR95HF												0x00
#define CR95HF_POLLING_TIMEOUT											0x01

// CR95HF polling status
#define CR95HF_IDLERES_LENGTH												0x01
#define CR95HF_WAKEUPSOURCE_TIMEOUT									0x01
#define CR95HF_WAKEUPSOURCE_TAGDETECTING						0x02
#define CR95HF_WAKEUPSOURCE_IRRQINN									0x08

// error code
#define	CR95HF_ERRORCODE_DEFAULT										0xFE
#define	CR95HF_ERRORCODE_TIMEOUT										0xFD
#define	CR95HF_ERRORCODE_UARTDATARATEUNCHANGED			0xFC
#define	CR95HF_ERRORCODE_UARTDATARATEPROCESS				0xFB
#define	CR95HF_ERRORCODE_FILENOTFOUND								0xFA
#define	CR95HF_ERRORCODE_READALLMEMORY							0xF9
#define	CR95HF_ERRORCODE_TAGDETECTINGCALIBRATION		0xF8
#define	CR95HF_ERRORCODE_CUSTOMCOMMANDUNKNOWN				0xF7
#define	CR95HF_ERRORCODE_TAGDETECTING								0xF5
#define	CR95HF_ERRORCODE_NOTAGFOUND									0xF4
#define CR95HF_ERROR_CODE														0x40
#define CR95HF_ERRORCODE_PARAMETERLENGTH						0x41
#define CR95HF_ERRORCODE_PARAMETER									0x42
#define CR95HF_ERRORCODE_COMMANDUNKNOWN							0x43
#define CR95HF_ERRORCODE_PORERROR										0x44


#define CR95HF_ERRORCODE_OUTOFMEMORY								0xEF
#define CR95HF_WRITE_MEMEORY_IN_MSD_FILE						0xDD

/* Nb of bytes of reader commands */
#define SELECT_BUFFER_SIZE									6
#define SENDRECV_BUFFER_SIZE								257
#define SEND_BUFFER_SIZE										257
#define IDLE_BUFFER_SIZE										16
#define RDREG_BUFFER_SIZE										5
#define WRREG_BUFFER_SIZE										257
#define BAUDRATE_BUFFER_SIZE								3
#define SUBFREQRES_BUFFER_SIZE							2
#define ACFILTER_BUFFER_SIZE								19
#define TESTMODE_BUFFER_SIZE								4
#define SLEEPMODE_BUFFER_SIZE								4
/* Nb of bytes of reader response */
#define CR95HF_RESPONSEBUFFER_SIZE		 			255


/* mask of steclect commnan */
#define CR95HF_SELECTMASK_DATARATE					0x30
#define CR95HF_SELECTMASK_SUBCARRIER				0x02
#define CR95HF_SELECTMASK_WAITORSOF					0x08


/* protocol allowed */
#define PROTOCOL_TAG_FIELDOFF								0x00
#define PROTOCOL_TAG_ISO15693								0x01
#define PROTOCOL_TAG_ISO14443A							0x02
#define PROTOCOL_TAG_ISO14443B							0x03
#define PROTOCOL_TAG_FELICA									0x04
							

#define PROTOCOL_TAG_ISO14443_SR							0x05
#define PROTOCOL_TAG_TOPAZ										0x06



#define PROTOCOL_CARD_ISO14443A								0x12
#define PROTOCOL_CARD_ISO14443B								0x13
#define PROTOCOL_CARD_FELICA									0x14

	
/* Offset definitions for global buffers */
#define CR95HF_COMMAND_OFFSET										0x00
#define CR95HF_LENGTH_OFFSET										0x01
#define CR95HF_DATA_OFFSET											0x02

#define CR95HF_TRANSPARENTCOMMAND_OFFSET				0x01

#define TAGREPPLY_OFFSET_UID						CR95HF_DATA_OFFSET+0x02

#define ECHOREPLY_OFFSET												0x00
	
#define PSEUDOREPLY_OFFSET											0x00
#define PSEUDOREPLY_LENGTH											0x01

#define READERREPLY_STATUSOFFSET								0x00

#define IDN_RESULTSCODE_OK											0x00
// protocol select status
#define PROTOCOLSELECT_LENGTH										0x02
#define PROTOCOLSELECT_RESULTSCODE_OK						0x00
#define PROTOCOLSELECT_ERRORCODE_CMDLENGTH			0x82
#define PROTOCOLSELECT_ERRORCODE_INVALID				0x83
// baud rate command
#define	BAUDRATE_LENGTH						  						0x01
#define	BAUDRATE_DATARATE_DEFAULT			  				57600
//  poll field status
#define POLLFIELD_RESULTSCODE_OK								0x00
//  send receive field status
#define SENDRECV_RESULTSCODE_OK									0x80
#define SENDRECV_ERRORCODE_COMERROR							0x86
#define SENDRECV_ERRORCODE_FRAMEWAIT						0x87
#define SENDRECV_ERRORCODE_SOF									0x88
#define SENDRECV_ERRORCODE_OVERFLOW							0x89
#define SENDRECV_ERRORCODE_FRAMING							0x8A
#define SENDRECV_ERRORCODE_EGT									0x8B
#define SENDRECV_ERRORCODE_LENGTH								0x8C
#define SENDRECV_ERRORCODE_CRC									0x8D
#define SENDRECV_ERRORCODE_RECEPTIONLOST				0x8E


#define SENDRECV_ERRORCODE_SOFT									0xFF


//  Listen command field status
#define LISTEN_RESULTSCODE_OK										0x00
#define LISTEN_ERRORCODE_CMDLENGTH							0x82
#define LISTEN_ERRORCODE_PROTOCOL								0x83
#define LISTEN_ERRORCODE_NOFIELD								0x8F
#define LISTEN_ERRORCODE_COM										0x86
#define LISTEN_ERRORCODE_CANCELED								0x87
#define LISTEN_ERRORCODE_SOF										0x88
#define LISTEN_ERRORCODE_OVERFLOW								0x89
#define LISTEN_ERRORCODE_FRAMING								0x8A
#define LISTEN_ERRORCODE_EGT										0x8B
#define LISTEN_ERRORCODE_CRC										0x8D
#define LISTEN_ERRORCODE_RECEPTIONLOST					0x8E
//  Send command field status
#define SEND_RESULTSCODE_OK											0x00
#define SEND_ERRORCODE_LENGTH										0x82
#define SEND_ERRORCODE_PROTOCOL									0x83
//  Idle command field status
#define IDLE_RESULTSCODE_OK											0x00
#define IDLE_ERRORCODE_LENGTH										0x82
//  read register command field status
#define READREG_RESULTSCODE_OK									0x00
#define READREG_ERRORCODE_LENGTH								0x82
//  write register command field status
#define WRITEREG_RESULTSCODE_OK									0x00
//  Baud rate command field status
#define BAUDRATE_RESULTSCODE_OK									0x55
//  AC filter command field status
#define ACFILTER_RESULTSCODE_OK									0x00
#define ACFILTER_ERRORCODE_LENGTH								0x82
//  sub freq command field status
#define SUBFREQ_RESULTSCODE_OK									0x00
//  Test mode command field status
#define TESTMODE_RESULTSCODE_OK									0x00
// ECHO response
#define ECHORESPONSE														0x55

/* control byte according to protocl*/
#define CONTROL_MAX_NBBYTE											0x03
#define CONTROL_15693_NBBYTE										0x01
#define CONTROL_15693_CRCMASK										0x02
#define CONTROL_15693_COLISIONMASK							0x01

#define CONTROL_14443A_NBBYTE										0x03
#define CONTROL_14443A_COLISIONMASK							0x80
#define CONTROL_14443A_CRCMASK									0x20
#define CONTROL_14443A_PARITYMASK								0x10
#define CONTROL_14443A_NBSIGNIFICANTBITMASK			0x0F
#define CONTROL_14443A_FIRSTCOLISIONBITMASK			0x0F

#define CONTROL_14443B_NBBYTE										0x01
#define CONTROL_14443B_CRCMASK									0x02
#define CONTROL_14443B_COLISIONMASK							0x01

#define CONTROL_FELICA_NBBYTE										0x01
#define CONTROL_FELICA_CRCMASK									0x02
#define CONTROL_FELICA_COLISIONMASK							0x01

// get MCU verion
#define MCUVERSION_STATUS												0x00
#define MCUVERSION_LENGTH												0x03
#define MCUVERSION_FIRSTNUMBER									2	// stand for 2.1.2
#define MCUVERSION_SECONDNUMBER									1
#define MCUVERSION_THIRDNUMBER									2

										  	

#define ERRORCODE_LENGTH												0x01

#define DUMMY_BYTE															0xFF
#define MAX_BUFFER_SIZE  												256 

#define CR95HF_FLAG_DATA_READY									0x08
#define CR95HF_FLAG_DATA_READY_MASK							0xF8


#define Memory_Bank_1 													0x00
#define Memory_Bank_2 													0x01

//ERROR CRC 

#define OFFSET_LENGTH														CR95HF_LENGTH_OFFSET
#define CRC_MASK																0x02
#define CRC_ERROR_CODE													0x02
#define COLISION_MASK														0x01
#define COLISION_ERROR_CODE											0x01

#define CRC_MASK_TYPE_A													0x20
#define CRC_ERROR_CODE_TYPE_A										0x20


					 
/* Functions ---------------------------------------------------------------- */

//  command prototypes

int8_t CR95HF_IsReaderResultCodeOk 		(uint8_t CmdCode,uc8 *ReaderReply);
int8_t CR95HF_IsReaderErrorCode 			(uint8_t CmdCode,uint8_t *ReaderReply);
int8_t CR95HF_IsCommandExists					(uint8_t CmdCode);


int8_t SplitReaderReply 							(uint8_t CmdCodeToReader,u8 ProtocolSelected,uc8 *ReaderReply,uint8_t *ResultCode,uint8_t *NbTagByte,uint8_t *TagReplyData,uint8_t *NbControlByte,uint8_t *ControlByte);



int8_t 	SPIUART_SendReceive						(uc8 *pCommand, uint8_t *pResponse);
void 	SPIUART_Send										(uc8 *pCommand);
int8_t 	CR95HF_CheckSendReceive				(uc8 *pCommand, uint8_t *pResponse);

void CR95HF_FieldOff								( void );
void CR95HF_FieldOn 								( void );

int8_t GetIRQOutState ( void );

int8_t CR95HF_IDN				( uint8_t *pResponse);
int8_t CR95HF_Echo			( uint8_t *pResponse);
int8_t CR95HF_SendEOF		( uint8_t *pResponse);
int8_t CR95HF_ProtocolSelect(uc8 Length,uc8 Protocol,uc8 *Parameters,uint8_t *pResponse);
int8_t CR95HF_SendRecv		( uc8 Length,uc8 *Parameters,uint8_t *pResponse);
int8_t CR95HF_Send				(uc8 Length,uc8 *Parameters);
int8_t CR95HF_Idle				( uc8 Length,uc8 *Data );
int8_t CR95HF_RdReg				( uc8 Length,uc8 Address,uc8 RegCount,uc8 Flags,uint8_t *pResponse);
int8_t CR95HF_WrReg 			( uc8 Length,uc8 Address,uc8 Flags,uc8 *pData,uint8_t *pResponse);
int8_t CR95HF_BaudRate 		( uc8 BaudRate,uint8_t *pResponse);

int8_t CR95HF_Run15693Anticollision (uc8 *pCommand, uint8_t *pResponse) ;
int8_t CR95HF_RunInventoty16slots 	(uc8 *pCommand, uint8_t *pResponse );


void CR95HF_SendCustomCommand		( uc8 *pCommand, uint8_t *pResponse);
void CR95HF_GetMCUversion				( uint8_t *pResponse );
void CR95HF_GetHardwareVersion	( uint8_t *pResponse );
void CR95HF_GetInterfaceBus 		( uint8_t *pResponse );
void CR95HF_SetUSBDisconnectPin ( void );
int8_t CR95HF_PORsequence				( void );
void CR95HF_Receive_SPI_Response( uint8_t *pData );

int8_t SPIUART_PoolingReading 			( uint8_t *pResponse);
int8_t CR95HF_ChangeUARTBaudRate 		( uc8 *pCommand, uint8_t *pResponse);
int8_t CR95HF_CalibrateTagDetection	(uc8 *pCommand, uint8_t *pResponse );

int8_t SpyTransaction				( uc8 *pCommand, uint8_t *pResponse);

#ifdef USE_POOL_FIELD
int8_t CR95HF_Pool_Field (uint8_t *pDataRead);
#endif /*USE_POOL_FIELD*/

#ifdef USE_MSD_DRIVE
uint8_t Read_MSD_File  ( uint8_t *File_Data, uint16_t *Length, const char *File_Name);
uint8_t Write_MSD_File ( uint8_t *File_Data, uint16_t Length, const char *File_Name);
uint8_t USB_MSD_Reboot ( void );
#endif /*USE_MSD_DRIVE*/


#endif /* __CR95HF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



