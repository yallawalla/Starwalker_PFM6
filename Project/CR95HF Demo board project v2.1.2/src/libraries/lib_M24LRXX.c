/**
  ******************************************************************************
  * @file    lib_M24LRXX.c 
  * @author  MMY Application Team
  * @version V0.1
  * @date    30/03/2011
  * @brief   This file provides set of firmaware functions to manage communication 
  * @brief   between CR95HF and M24LRXX RFID card
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

#include "stm32f10x.h"
#include "lib_M24LRXX.h"
#include "lib_iso15693.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
//#include "main.h"

#include "hw_config.h"
//#include "stm32f10x_gpio.h"
//#include "menu.h"
//#include "usb_pwr.h"
//#include "usb_lib.h"
//#include "usb_hid.h"
//#include "lib_card_manager.h"
//#include "drv_lcd.h"
//#include "drv_LED.h"
#include "string.h"
#include "stdlib.h"
//#include "lib_CR95HF.h"
//#include "lib_iso15693.h"
//#include "lib_iso14443A.h"
//#include "lib_iso14443A.h"
//#include "lib_iso14443SR.h"
//#include "lib_felica.h"
//#include "lib_M24LRXX.h"
#include "lib_iso15693_LRIS2k.h"
#include "lib_NDEF.h"
//#include "miscellaneous.h"
//#include "lib_pff.h"
//#include "lib_iso14443A_card.h"

#include "usb_type.h"

#define M24LRXX_CRC16 ISO15693_CRC16
#define M24LRXX_IsCorrectCRC16Residue ISO15693_IsCorrectCRC16Residue

/** @addtogroup library
 * 	@{
 */

 /** @addtogroup DualM24LR64
 * 	@{
 */


/** @defgroup ISO15693_Private_Functions
 *  @{
 */

//static int8_t M24LRXX_IsCorrectCRC16Residue (uc8 *DataIn,uc8 Length);
//static int16_t M24LRXX_CRC16 (uc8 *DataIn,uc8 NbByte);
static int8_t M24LRXX_SplitBasicResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
static int8_t M24LRXX_IsReaderConfigMatchWithFlag( uc8 ParameterSelected , uc8 Flags);
static int8_t M24LRXX_IsReaderConfigMatchWithFlagWriteCmd( uc8 ParameterSelected , uc8 Flags);
static int8_t M24LRXX_IsFastReaderConfig ( uc8 ParameterSelected);

extern void delay_ms(uint16_t delay);
extern uint8_t GloParameterSelected;
extern uint8_t 	ReaderRecBuf[MAX_BUFFER_SIZE+3];

 /**  
* @brief  this function selects 15693 protocol accoording to input parameters
* @param  	DataRate	:  	tag data rate ( 6 or 26 or 52k)
* @param 	TimeOrSOF	: 	wait for SOF or respect 312 µs delay
* @param	Modulation	: 	10 or 100 %modulation depth
* @param	SubCarrier	: 	single or double sub carrier
* @param	AppendCRC	: 	if = 1 the reader computes the CRC command
* @retval status function
*/
int8_t M24LRXX_SelectProtocol (uc8 DataRate, uc8 TimeOrSOF, uc8 Modulation, uc8 SubCarrier, uc8 AppendCRC)
{
	return ISO15693_ProtocolSelectProtocol	(DataRate, TimeOrSOF, Modulation, SubCarrier, AppendCRC);
}


/**  
* @brief  this function return a Byte, which is concatenation of iventory flags 
* @param  	SubCarrierFlag	:  
* @param	DataRateFlag
* @param	InventoryFlag
* @param	ProtExtFlag
* @param	SelectOrAFIFlag
* @param	AddrOrNbSlotFlag
* @param	OptionFlag
* @param	RFUFlag
* @retval 	Flags byte
*/
int8_t M24LRXX_CreateRequestFlag (uc8 SubCarrierFlag,uc8 DataRateFlag,uc8 InventoryFlag,uc8 ProtExtFlag,uc8 SelectOrAFIFlag,uc8 AddrOrNbSlotFlag,uc8 OptionFlag,uc8 RFUFlag)
{

	return (ISO15693_CreateRequestFlag (
						SubCarrierFlag,
						DataRateFlag,
						InventoryFlag,
						ProtExtFlag,
						SelectOrAFIFlag,
						AddrOrNbSlotFlag,
						OptionFlag,
						RFUFlag));
}


/**  
* @brief  this function send an inventory command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	tag reply
* @retval status function
*/
int8_t M24LRXX_Inventory( uc8 Flags , uc8 AFIin, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  )
{
uint8_t 	M24LRXX_Flags;
	
	// force inventory flag to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	return ISO15693_Inventory(	M24LRXX_Flags ,
							AFIin,
							MaskLength,
							MaskValue,
							AppendCRC,
							CRC16,
							pResponse  );

}

/**  
* @brief  this function send an inventory command  with the NbSlot flag force to One.
	it returns RESULTOK if the command was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t M24LRXX_InventoryOneSlot( uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  )
{
int8_t 	NewFlags;
	
	// force NbSlot Flag to 1;
	NewFlags = Flags | ISO15693_MASK_ADDRORNBSLOTSFLAG;
	 
	return M24LRXX_Inventory( 	NewFlags ,
								AFI, 
								MaskLength, 
								MaskValue, 
								AppendCRC, 
								CRC16, 
								pResponse  );
}

/**  
* @brief  this function send an inventory command  with the NbSlot flag force to zero.
	it returns RESULTOK if the command was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	NbTag		:	Number of tag seen 
* @param	pResponse	:  	pointer of NbTag
* @retval 	Slot occupied (bit 0 = slot 0;bit 1 = slot 1;... )
*/
int16_t M24LRXX_Inventory16Slots( uc8 Flags , uc8 AFIin, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *NbTag, uint8_t *pResponse  )
{
	
return (ISO15693_Inventory16Slots( 	Flags ,
									AFIin,
									MaskLength, 
									MaskValue, 
									AppendCRC, 
									CRC16, 
									NbTag, 
									pResponse  ));

}

/**  
* @brief  this function send an EOF pulse
* @param  	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_SendEOF(uint8_t *pResponse  )
{	
	return ISO15693_SendEOF(pResponse);

}

/**  
* @brief  this function returns RESULTOK if request flag is coherent with parameter of
	the select command (reader command). the subcarrier and data rate shall be 
	egual.	
* @param  	Flags					:  	inventory flags
			GloParameterSelected	:	
* @retval status function
*/
static int8_t M24LRXX_IsReaderConfigMatchWithFlag( uc8 ParameterSelected , uc8 Flags)
{
				  
	if ((ParameterSelected & CR95HF_SELECTMASK_DATARATE) != (ISO15693_TRANSMISSION_53<<4))
	{	
		// (acc to ISO spec) Data rate flag = 0 => low data rates
		// (acc to reader datasheet) Data rate value = 0b10 => 6k (low data rate)
	   	if ( (ISO15693_GetDataRateFlag (Flags) == FALSE) & 
			 (ParameterSelected & CR95HF_SELECTMASK_DATARATE) != (ISO15693_TRANSMISSION_6<<4) )
			 return ERRORCODE_GENERIC;
		
	// (acc to ISO spec) Data rate flag = 1 => high data rates
		// (acc to reader datasheet) Data rate value = 0b00 => 26k (high data rate)
		if ( (ISO15693_GetDataRateFlag (Flags) == TRUE) & 
			 (ParameterSelected & CR95HF_SELECTMASK_DATARATE) != ISO15693_TRANSMISSION_26)
			 return ERRORCODE_GENERIC;
	}

	// Sub carrier flag shall be equal to Sub carrrier value
	if ( (ISO15693_GetSubCarrierFlag (Flags) == TRUE) & 
		 (ParameterSelected & CR95HF_SELECTMASK_SUBCARRIER) != CR95HF_SELECTMASK_SUBCARRIER )
		 return ERRORCODE_GENERIC;
	if ( (ISO15693_GetSubCarrierFlag (Flags) == FALSE) & 
		 (ParameterSelected & CR95HF_SELECTMASK_SUBCARRIER) != 0 )
		 return ERRORCODE_GENERIC;	  

		
	return RESULTOK;

}

/**  
* @brief  this function returns RESULTOK if request flag is coherent with parameter of
	the select command (reader command). the subcarrier and data rate shall be 
	egual.	
* @param  	Flags					:  	inventory flags
			GloParameterSelected	:	
* @retval status function
*/
static int8_t M24LRXX_IsFastReaderConfig ( uc8 ParameterSelected)
{
	// (acc to reader datasheet) Data rate value = 0b10 => 6k (low data rate)
   	if ( (ParameterSelected & CR95HF_SELECTMASK_DATARATE) != (ISO15693_TRANSMISSION_53<<4) )
		 return ERRORCODE_GENERIC;

	return RESULTOK;
}

/**  
* @brief  this function returns RESULTOK if request flag is coherent with parameter of
	the select command (reader command). the subcarrier and data rate shall be 
	egual.	
* @param  	Flags					:  	inventory flags
			GloParameterSelected	:	parameter of select command
* @retval status function
*/
static int8_t M24LRXX_IsReaderConfigMatchWithFlagWriteCmd( uc8 ParameterSelected , uc8 Flags)
{
int8_t status;

	errchk ( ISO15693_IsReaderConfigMatchWithFlagWriteCmd( ParameterSelected , Flags));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitInventoryResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitFastInventoryInitiatedResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitInventoryInitiatedResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitFastInitiateResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitInitiateResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t M24LRXX_SplitGetSystemInfoResponse( uc8 *ReaderResponse,uc8 Length,uint8_t *Flags ,uint8_t *InfoFlags,uint8_t *UIDout,uint8_t *DSFIDout,uint8_t *AFIout,uint8_t *MemSizeOut,uint8_t *IcRefOut,uint8_t *ErrorCode)
{
int8_t 	NbByte = ISO15693_GETSYSINFOOFFSET_DSFID,
		status; 
uint8_t	ResultCode,
		NbTagReplyByte,
		NbControlByte,
		TagReplyIndex,
		ControlIndex;

	SplitReaderReply (	SEND_RECEIVE,
						PROTOCOL_TAG_ISO15693,
						ReaderResponse,
						&ResultCode,
						&NbTagReplyByte,
						&TagReplyIndex,
						&NbControlByte,
						&ControlIndex);

	if (ResultCode == CR95HF_ERROR_CODE)
	{
		*ErrorCode = CR95HF_ERROR_CODE; 
		//*DataOutIndex = READERREPLY_STATUSOFFSET;
		return CR95HF_ERROR_CODE;	
	}

	*IcRefOut = 0xFF;
	*DSFIDout =	0xFF;
	errchk(M24LRXX_IsCorrectCRC16Residue (&ReaderResponse[TagReplyIndex],Length));
	
	*Flags = ReaderResponse[TagReplyIndex+ISO15693_OFFSET_FLAGS];

	if (ISO15693_IsTagErrorCode (*Flags) == RESULTOK)
	{	*ErrorCode = ReaderResponse[TagReplyIndex+NbByte++];
		memset (MemSizeOut,0x00,M24LRXX_NBBYTE_MEMORYSIZE);
		return RESULTOK;		
	}

	*InfoFlags = ReaderResponse[TagReplyIndex+ISO15693_GETSYSINFOOFFSET_INFOFLAGS];
	
	memcpy(UIDout,&(ReaderResponse[TagReplyIndex+ISO15693_GETSYSINFOOFFSET_UID]),ISO15693_NBBYTE_UID);
	
	if (ISO15693_GetDSFIDFlag (*InfoFlags) == RESULTOK)
		*DSFIDout = ReaderResponse[TagReplyIndex+NbByte++];
	if (ISO15693_GetAFIFlag (*InfoFlags) == RESULTOK)
		*AFIout = ReaderResponse[TagReplyIndex+NbByte++];
	if (ISO15693_GetMemorySizeFlag (*InfoFlags) == RESULTOK)
	{
		MemSizeOut[0] = ReaderResponse[TagReplyIndex+NbByte++]; 
		MemSizeOut[1] = ReaderResponse[TagReplyIndex+NbByte++]; 
		MemSizeOut[2] = ReaderResponse[TagReplyIndex+NbByte++]; 
	}
	if (ISO15693_GetICReferenceFlag (*InfoFlags) == RESULTOK)
		*IcRefOut = ReaderResponse[TagReplyIndex+NbByte++]; 
		
	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	Data read into tag memory (block length (depending of tag))
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitReadSingleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadSingleBlockResponse( TagResponse,Length,NbByteOfData,OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	Data read into tag memory (block length (depending of tag))
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitFastReadSingleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadSingleBlockResponse( TagResponse,Length,NbByteOfData,OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	Data read into tag memory (block length (depending of tag))
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_FastSplitReadSingleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadSingleBlockResponse( TagResponse,Length,NbByteOfData,OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
	
}

/**  
* @brief  this function splits Read Multiple Blocks response. 
	If the residue of tag response	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	NbBlock		 : 	Number of block to read
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	Data read into tag memory (block length (depending of tag))
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitReadMultipleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadMultipleBlockResponse( TagResponse,Length, NbBlock, NbByteOfData, OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
}
/**  
* @brief  this function splits Read Multiple Blocks response. 
	If the residue of tag response	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	NbBlock		 : 	Number of block to read
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	Data read into tag memory (block length (depending of tag))
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitFastReadMultipleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadMultipleBlockResponse( TagResponse,Length, NbBlock, NbByteOfData, OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
}


/**  
* @brief  this function splits GetMultiple Block Secutity status response. 
	If the residue of tag response	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	NbByteOfData : 	Block size memeory 	(depending of tag)
* @param	NbBlock		 : 	Number of block to read
* @param	OptionFlag	: 	option flag in request flags
* @param	Flags		:  	response flag (1 byte)
* @param	SecurityStatus : block security status (optionl) (1 byte)
* @param	Data		:	block security status (NbBlock * 8bits)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitGetMultipleBlockSecutityResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uint8_t *Flags ,uint8_t *DataOut,uint8_t *ErrorCode)
{

	return ISO15693_SplitGetMultipleBlockSecutityResponse( TagResponse, Length, NbBlock,Flags ,DataOut,ErrorCode);

}

/**  * @brief  this function splits Write single block response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitWriteSingleBlockResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitWriteSingleBlockResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Write sector password response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitWriteSectorPasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return M24LRXX_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits lock sector password response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitLockSectorPasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return M24LRXX_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits present sector password response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitPresentSectorPasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return M24LRXX_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Write AFI response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitWriteAFIResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitWriteAFIResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Write DSFID response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitWriteDSFIDResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitWriteDSFIDResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits reset To Ready response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitResetToReadyResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitResetToReadyResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Select response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitSelectResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitSelectResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Lcok block response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitLockBlockResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitLockBlockResponse( TagResponse,Length,Flags ,ErrorCode);

}

/**  * @brief  this function splits Lcok AFI response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitLockAFIResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitLockAFIResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Lcok AFI response. If the residue of tag response
	is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t M24LRXX_SplitLockDSFIDResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitLockDSFIDResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits tag response, which is composed with flags and CRC16 
(if error flag is not set) or with Flags and error code (if error flag is set).
If the residue of tag response is incorrect the function returns FALSE, otherwise RESULTOK
		was correctly emmiting, FALSE otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
static int8_t M24LRXX_SplitBasicResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}


/**  
* @brief  this function send an stay_quiet command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t M24LRXX_StayQuiet(uc8 Flags,uc8 *UIDin,uc8 AppendCRC,uc8 *CRC16)
{	
	return ISO15693_StayQuiet(Flags,UIDin,AppendCRC,CRC16);	 
}		

/**  
* @brief  this function send an ReadSingleBlock command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_ReadSingleBlock (uc8 Flags, uc8 *UID, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_READSINGLEBLOCK],
		NthByte=0;
int8_t status,
		i,
		M24LRXX_Flags;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;

	errchk(M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));		

	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;
	

	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_READSINGLEBLOCK;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	for (i = 0;i<ISO15693_NBBYTE_UID;i++)
		 DataToSend[NthByte++] =  UID[i];
	}
	
	// block number is 16 bits word	
	DataToSend[NthByte++] = BlockNumber[0];
	DataToSend[NthByte++] = BlockNumber[1];

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == RESULTOK)
		return RESULTOK;
	else 
		return ERRORCODE_GENERIC;

Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function send a WriteSingleblock command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	: 
* @param	BlockLength :	Nb of byte of block length
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_WriteSingleBlock(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 BlockLength,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_WRITESINGLEBLOCK],
		NthByte=0,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;	

	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));
	errchk (M24LRXX_IsReaderConfigMatchWithFlagWriteCmd (GloParameterSelected,M24LRXX_Flags));

	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;
	
	// block number is 16 bits word	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_WRITESINGLEBLOCK;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	DataToSend[NthByte++] = BlockNumber[0];
	DataToSend[NthByte++] = BlockNumber[1];

	memcpy(&(DataToSend[NthByte]),DataToWrite,BlockLength);
	NthByte +=BlockLength;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == RESULTOK)
		return RESULTOK;
	else 
		return ERRORCODE_GENERIC;

//	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function send a WriteSectorPassword command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	PasswordNumber	:	this value si betwenn 1 and 3 
* @param	Password	 :	32 bytes word
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_WriteSectorPassword(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_WRITESECTORPWD],
		NthByte=0;
int8_t	status;

	
	errchk (CHECKVAL(PasswordNumber,M24LRXX_RFPASSWORD_MIN,M24LRXX_RFPASSWORD_MAX));
	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (M24LRXX_IsReaderConfigMatchWithFlagWriteCmd (GloParameterSelected,Flags));
	
	
	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_WRITESECTORPWD;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	DataToSend[NthByte++] = PasswordNumber;

	memcpy(&(DataToSend[NthByte]),Password,M24LRXX_RFPASSWORD_NBBYTE);
	NthByte += M24LRXX_RFPASSWORD_NBBYTE;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Lock Sector Password command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	SectorNumber	:	this value is betwenn 0 and 63(2 bytes word)
* @param	SectorSecurityStatus	 :	1 byte word
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_LockSectorPassword(uc8 Flags, uc8 *UIDin, uc8 *SectorNumber,uc8 SectorSecurityStatus,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_LOCKSECTOR],
		NewSectorNumber[M24LRXX_SECTORNUMBER_NBBYTE],
		NthByte=0,
		NewSectorSecurityStatus,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;	

	NewSectorNumber [0] = SectorNumber[0];
	NewSectorNumber	[1] = 0;

	if (NewSectorNumber[0] > M24LRXX_SECTOR_MAX)
		return ERRORCODE_GENERIC;
	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));
	errchk (M24LRXX_IsReaderConfigMatchWithFlagWriteCmd (GloParameterSelected,M24LRXX_Flags));
  	
	// set bit 0 to 1 of SectorSecurityStatus  
	NewSectorSecurityStatus = SectorSecurityStatus | M24LRXX_MASK_SECTORSECURITYSTATUS_LOCKBIT;
	// set bit 7 to 5 to 0 of SectorSecurityStatus
	NewSectorSecurityStatus &= M24LRXX_MASK_SECTORSECURITYSTATUS_USEDBIT;	
	
	// block number is 16 bits word	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_LOCKSECTORPWD;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	
	DataToSend[NthByte++] =NewSectorNumber [0];
	DataToSend[NthByte++] =NewSectorNumber [1];

	DataToSend[NthByte++] = NewSectorSecurityStatus;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Present Sector Password command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	PasswordNumber	:	this value si betwenn 1 and 3 
* @param	Password	 :	32 bits word
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_PresentSectorPassword(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_PRESENTSECTORPWD],
		NthByte=0;
int8_t	status;

	errchk (CHECKVAL(PasswordNumber,M24LRXX_RFPASSWORD_MIN,M24LRXX_RFPASSWORD_MAX));
	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));

	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_PRESENTSECTORPWD;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	
	DataToSend[NthByte++] =PasswordNumber;

	memcpy(&(DataToSend[NthByte]),Password,M24LRXX_RFPASSWORD_NBBYTE);
	NthByte +=M24LRXX_RFPASSWORD_NBBYTE;


	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Fast Read Single Block command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	:	this value is between 1 and 2047
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_FastReadSingleBlock (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_FASTREADSINGLEBLOCK],
		NthByte=0,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;

	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));
	errchk (M24LRXX_IsFastReaderConfig (GloParameterSelected));

	
	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;
  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_FASTREADSINGLEBLOCK;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	
	DataToSend[NthByte++] =BlockNumber[0];
	DataToSend[NthByte++] =BlockNumber[1];

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function send a Fast Inventoru Initiated command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:  	AfI	(optional)
* @param	MaskLength	:	this value si betwenn 1 and 64 
* @param	Mask	 	:	64 bits word
* @param	AppendCRC	:	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_FastInventoryInitiated (uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_FASTINVENTORYINITIATED],
		NthByte=0,
		NbMaskBytes = 0,
		NbSignificantBits=0,
		M24LRXX_Flags;
int8_t	FirstByteMask,
		NthMaskByte = 0,
		status;
	
	// the inventory flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	errchk(ISO15693_GetInventoryFlag (M24LRXX_Flags));
	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags))	
	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_FASTINVENTORYINITIATED;
	DataToSend[NthByte++] = M24LRXX_ICCODE;
	
	if (ISO15693_GetSelectOrAFIFlag (M24LRXX_Flags) == RESULTOK)
		DataToSend[NthByte++] = AFIin;
	
	DataToSend[NthByte++] = MaskLength;

	if (MaskLength !=0)
	{
		// compute the number of bytes of mask value(2 border exeptions)
	   	if (MaskLength == 64)
			NbMaskBytes = 8;
		else
	   		NbMaskBytes = MaskLength / 8 + 1;
	
		NbSignificantBits = MaskLength - (NbMaskBytes-1) * 8;
		if (NbSignificantBits !=0)
	   		FirstByteMask = (0x01 <<NbSignificantBits)-1;
		else 
			FirstByteMask = 0xFF;
	
	   	// copy the mask value 
		if (NbMaskBytes >1)
		{
			for (NthMaskByte = 0; NthMaskByte < NbMaskBytes - 1; NthMaskByte ++ )
				DataToSend[NthByte++] = Mask[NthMaskByte];
		}
	
		if (NbSignificantBits !=0)
			DataToSend[NthByte++] = Mask[NthMaskByte] & FirstByteMask;
	}

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	
		DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}
 	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == FALSE)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	// initialize the result code to 0xFF and length to 0  in case of error 
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Inventoru Initiated command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:  	AfI	(optional)
* @param	MaskLength	:	this value si betwenn 1 and 64 
* @param	Mask	 	:	64 bits word
* @param	AppendCRC	:	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_InventoryInitiated (uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_INVENTORYINITIATED],
		NthByte=0,
		NbMaskBytes = 0,
		NbSignificantBits=0,
		M24LRXX_Flags;
int8_t	FirstByteMask,
		NthMaskByte = 0,
		status;
	
	// the inventory flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	errchk(ISO15693_GetInventoryFlag (M24LRXX_Flags));
	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags))	
	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_INVENTORYINITIATED;
	DataToSend[NthByte++] = M24LRXX_ICCODE;
	
	if (ISO15693_GetSelectOrAFIFlag (M24LRXX_Flags) == RESULTOK)
		DataToSend[NthByte++] = AFIin;
	
	DataToSend[NthByte++] = MaskLength;

	if (MaskLength !=0)
	{
		// compute the number of bytes of mask value(2 border exeptions)
	   	if (MaskLength == 64)
			NbMaskBytes = 8;
		else
	   		NbMaskBytes = MaskLength / 8 + 1;
	
		NbSignificantBits = MaskLength - (NbMaskBytes-1) * 8;
		if (NbSignificantBits !=0)
	   		FirstByteMask = (0x01 <<NbSignificantBits)-1;
		else 
			FirstByteMask = 0xFF;
	
	   	// copy the mask value 
		if (NbMaskBytes >1)
		{
			for (NthMaskByte = 0; NthMaskByte < NbMaskBytes - 1; NthMaskByte ++ )
				DataToSend[NthByte++] = Mask[NthMaskByte];
		}
	
		if (NbSignificantBits !=0)
			DataToSend[NthByte++] = Mask[NthMaskByte] & FirstByteMask;
	}

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	
		DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}
 	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == FALSE)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	// initialize the result code to 0xFF and length to 0  in case of error 
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Fast Initiate command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_FastInitiate (uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_FASTINITIATE],
		NthByte=0;
int8_t	status;


	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (M24LRXX_IsFastReaderConfig (GloParameterSelected));

  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_FASTINITIATE;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Fast Initiate command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_Initiate (uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_FASTINITIATE],
		NthByte=0;
int8_t	status;


	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (M24LRXX_IsFastReaderConfig (GloParameterSelected));

  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_INITIATE;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function send a Fast Read Multiple Block command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	:	first block number. this value is between 1 and 2047
* @param	NumberOfBlock	this value is between 1 and 32
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_FastReadMultipleBlock (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 NumberOfBlock,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_READMULTIPLEBLOCK],
		NthByte=0,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;

	errchk(CHECKVAL(NumberOfBlock,M24LRXX_MINBLOCK_READMULTIPLEBLOCK,M24LRXX_MAXBLOCK_READMULTIPLEBLOCK));
	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));
	errchk (M24LRXX_IsFastReaderConfig (GloParameterSelected));

	
	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;
  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = M24LRXX_CMDCODE_FASTREADMULTIPLEBLOCK;
	DataToSend[NthByte++] = M24LRXX_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	
	DataToSend[NthByte++] =BlockNumber[0];
	DataToSend[NthByte++] =BlockNumber[1];

	DataToSend[NthByte++] =NumberOfBlock;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk(CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a write AFI command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID (optional)
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t M24LRXX_WriteAFI(uc8 Flags, uc8 *UIDin,uc8 AFIToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{

	return ISO15693_WriteAFI(Flags, UIDin,AFIToWrite,AppendCRC, CRC16,pResponse );
}

/**  
* @brief  this function send a write DSFID command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID (optional)
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_WriteDSFID(uc8 Flags, uc8 *UIDin,uc8 DSFIDToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{

	return ISO15693_WriteDSFID(Flags, UIDin,DSFIDToWrite,AppendCRC, CRC16,pResponse );
}


/**  
* @brief  this function send an select command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_Select ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
	return ISO15693_Select ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}	

/**  
* @brief  this function send an select command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_ResetToReady ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{

	return ISO15693_ResetToReady ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}


/**  
* @brief  this function send an LockBlock command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_LockAFI ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse )
{
int8_t 	status;
uint8_t M24LRXX_Flags;
	
	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;
	errchk(M24LRXX_IsReaderConfigMatchWithFlagWriteCmd( GloParameterSelected ,M24LRXX_Flags));

	return ISO15693_LockAFI ( M24LRXX_Flags, UIDin, AppendCRC,CRC16,pResponse );
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send an Lock DSFID command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_LockDSFID ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{

	return ISO15693_LockDSFID ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}

/**  
* @brief  this function send an GetSystemInfo command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
uint8_t M24LRXX_Flags;
	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;
	return ISO15693_GetSystemInfo ( M24LRXX_Flags, UIDin, AppendCRC,CRC16,pResponse);
}

/**  
* @brief  this function send an stay_quit command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t M24LRXX_SplitMemorySizeInfo( uc8 *MemSizeInfo,uint8_t *BlockSize ,uint8_t *NbBlocks)
{

	NbBlocks[0] =  MemSizeInfo[0]+1;
	NbBlocks[1] =  MemSizeInfo[1]+1;
	*BlockSize = MemSizeInfo[2]+1  ;
	

	return RESULTOK;
}



/**  
* @brief  this function send an LockBlock command and returns RESULTOK if the command 
		was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	BlockNumber	: 
* @param	NbBlocks	: 	Number of blocks
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_ReadMultipleBlocks (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse)
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_READMULBLOCK],
		NthByte=0,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;
	
	errchk (M24LRXX_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));

	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;

	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_READMULBLOCKS;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	// block number is 16 bits word	
	DataToSend[NthByte++] = BlockNumber[0];
	DataToSend[NthByte++] = BlockNumber[1];

	DataToSend[NthByte++] = NbBlocks;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	errchk (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse));

	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Get Multiple Block Secutity Status  command and returns
 RESULTOK if the command was correctly emmiting, FALSE otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	BlockNumber	: 
* @param	NbBlocks	: 	Number of blocks
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t M24LRXX_GetMultipleBlockSecutityStatus (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 *NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse)
{
uint8_t DataToSend[M24LRXX_MAXLENGTH_GETMULSECURITY],
		NthByte=0,
		M24LRXX_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	M24LRXX_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;

	errchk (ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,M24LRXX_Flags));

	if (Uint8ToUint16 (BlockNumber)>M24LRXX_BLOCK_MAX)
		return ERRORCODE_GENERIC;

	DataToSend[NthByte++] = M24LRXX_Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_GETSECURITYINFO;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== RESULTOK)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	// block number is 16 bits word	
	DataToSend[NthByte++] = BlockNumber[0];
	DataToSend[NthByte++] = BlockNumber[1];
	// Nb block number is 16 bits word	
	DataToSend[NthByte++] = NbBlocks[0];
	DataToSend[NthByte++] = NbBlocks[1];

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == FALSE)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}


 
/**  
* @brief  this function return a tag UID 
* @param  	UIDout: 	UID of a tag in the field
* @retval status function
*/
int8_t M24LRXX_GetUID (uint8_t *UIDout) 
{ 	
	return ISO15693_GetUID (UIDout);
 	
}


/**  
* @brief  this function return a tag UID 
* @param  	NbBlock		: 	number of block of tag memory 
* @param	BlockSize	:	number of byte of a memory block
* @retval status function
*/
int8_t M24LRXX_GetMemSizeInfo (uint8_t *NbBlock,uint8_t *BlockSize) 
{ 
 int8_t 	FlagsByteData;
uint8_t	//TagReplyBuf	[ISO15693_MAXLENGTH_REPLYGETSYSTEMINFO],
//		ControlBuf [CONTROL_MAX_NBBYTE],
//		NbTagReplyByte,
//		NbControlByte,
		Flags,
		InfoFlags[ISO15693_NBBYTE_INFOFLAG],
//		ResultCode = 0,
		AFIout[ISO15693_NBBYTE_AFI],
		MemSizeOut[ISO15693_NBBYTE_MEMORYSIZE],
		IcRefOut[ISO15693_NBBYTE_ICREF],
		DSFIDout[ISO15693_NBBYTE_DSFID],
		UIDout[ISO15693_NBBYTE_UID],
		errorcode;
int8_t	status;

	*NbBlock=0;
	*BlockSize=0;

	FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
												ISO15693_REQFLAG_HIGHDATARATE,
												ISO15693_REQFLAG_INVENTORYFLAGNOTSET,
												ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
												ISO15693_REQFLAG_NOTSELECTED,
												ISO15693_REQFLAG_NOTADDRESSED,
												ISO15693_REQFLAG_OPTIONFLAGNOTSET,
												ISO15693_REQFLAG_RFUNOTSET);
	// select 15693 protocol
	errchk(M24LRXX_SelectProtocol	(	ISO15693_TRANSMISSION_26,
								ISO15693_WAIT_FOR_SOF,
								ISO15693_MODULATION_100,
								ISO15693_SINGLE_SUBCARRIER,
								ISO15693_APPENDCRC));

	//int8_t ISO15693_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
	// Uid and CRC = 0x00
	errchk(M24LRXX_GetSystemInfo (FlagsByteData,
							0x00,
							ISO15693_APPENDCRC,
							0x00,
							ReaderRecBuf));
	
				
//	SplitReaderReply (	SEND_RECEIVE,
//						PROTOCOL_TAG_ISO15693,
//						ReaderRecBuf,
//						&ResultCode,
//						&NbTagReplyByte,
//						TagReplyBuf,
//						&NbControlByte,
//						ControlBuf);

	errchk(M24LRXX_SplitGetSystemInfoResponse(	ReaderRecBuf,
											ReaderRecBuf[CR95HF_LENGTH_OFFSET],
											&Flags ,
											InfoFlags,
											UIDout,
											DSFIDout,
											AFIout,
											MemSizeOut,
											IcRefOut,
											&errorcode));

	if (ISO15693_IsTagErrorCode(Flags) == RESULTOK)
	{
		*NbBlock = 0;
		*(NbBlock+1) = 0;
		*BlockSize = 0;
		
		return RESULTOK;
	}

	M24LRXX_SplitMemorySizeInfo(MemSizeOut,BlockSize ,NbBlock);

	return RESULTOK;

Error:
	return ERRORCODE_GENERIC;
	

}

/**  
* @brief  this function returns RESULTOK if the tag is a M24LR64. the two last UID bytes shall be E0 02 (02 means 
	STM product) and the IC ref returned by get system info shall be 0x2C
		
* @param  	Mask		: 	mask used for inventory (optional)
* @param  	MaskLength	: 	mask length (nb of bit)
* @retval status function
*/
int8_t M24LRXX_IsAM24LR64 (uint8_t *Mask,uint8_t MaskLength) 
{ 
 int8_t 	FlagsByteData;
uint8_t	//TagReplyBuf	[ISO15693_MAXLENGTH_REPLYGETSYSTEMINFO],
///		ControlBuf [CONTROL_MAX_NBBYTE],
//		NbTagReplyByte,
//		NbControlByte,
		Flags,
//		AFIin=0x00,
		InfoFlags[ISO15693_NBBYTE_INFOFLAG],
//		ResultCode = 0,
		AFIout[ISO15693_NBBYTE_AFI],
		MemSizeOut[ISO15693_NBBYTE_MEMORYSIZE],
		IcRefOut,
		DSFIDout[ISO15693_NBBYTE_DSFID],
		UIDout[ISO15693_NBBYTE_UID],
		errorcode;
int8_t	status;

	if ( MaskLength > ISO15693_NBBYTE_UID * 8)
		return ERRORCODE_GENERIC;

	
	errchk(M24LRXX_GetUID (UIDout));


	if (UIDout[ISO15693_NBBYTE_UID-1]!=0xE0 || UIDout[ISO15693_NBBYTE_UID-2]!=M24LRXX_ICCODE)
		return ERRORCODE_GENERIC;
		

	FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
												ISO15693_REQFLAG_HIGHDATARATE,
												ISO15693_REQFLAG_INVENTORYFLAGNOTSET,
												ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
												ISO15693_REQFLAG_NOTSELECTED,
												ISO15693_REQFLAG_ADDRESSED,
												ISO15693_REQFLAG_OPTIONFLAGNOTSET,
												ISO15693_REQFLAG_RFUNOTSET);	

	//int8_t ISO15693_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
	// Uid and CRC = 0x00
	errchk(M24LRXX_GetSystemInfo (FlagsByteData,
							UIDout,
							ISO15693_APPENDCRC,
							0x00,
							ReaderRecBuf));
	
				
//	SplitReaderReply (	SEND_RECEIVE,
//						PROTOCOL_TAG_ISO15693,
//						ReaderRecBuf,
//						&ResultCode,
//						&NbTagReplyByte,
//						TagReplyBuf,
//						&NbControlByte,
//						ControlBuf);

	errchk(M24LRXX_SplitGetSystemInfoResponse(	ReaderRecBuf,
											ReaderRecBuf[CR95HF_LENGTH_OFFSET],
											&Flags ,
											InfoFlags,
											UIDout,
											DSFIDout,
											AFIout,
											MemSizeOut,
											&IcRefOut,
											&errorcode));

	if ( IcRefOut == M24LRXX_ICREF)
		return RESULTOK;;

	
	return ERRORCODE_GENERIC;

Error:
	return M24LRXX_ERROR_CODE;
}

/**  
* @brief  this function returns RESULTOK if the tag is a M24LR64. the two last UID bytes shall be E0 02 (02 means 
	STM product) and the IC ref returned by get system info shall be 0x2C
		
* @param  	Mask		: 	mask used for inventory (optional)
* @param  	MaskLength	: 	mask length (nb of bit)
* @retval status function
*/
int8_t M24LRXX_GetIcRef (uint8_t *IcRefOut) 
{ 
 int8_t 	FlagsByteData;
uint8_t	//TagReplyBuf	[ISO15693_MAXLENGTH_REPLYGETSYSTEMINFO],
//		ControlBuf [CONTROL_MAX_NBBYTE],
//		NbTagReplyByte,
//		NbControlByte,
		Flags,
//		AFIin=0x00,
		InfoFlags[ISO15693_NBBYTE_INFOFLAG],
//		ResultCode = 0,
		AFIout[ISO15693_NBBYTE_AFI],
		MemSizeOut[ISO15693_NBBYTE_MEMORYSIZE],
		DSFIDout[ISO15693_NBBYTE_DSFID],
		UIDout[ISO15693_NBBYTE_UID],
		errorcode;
int8_t	status;

	*IcRefOut =0xFF;
	
	errchk(M24LRXX_GetUID (UIDout));


	if (UIDout[ISO15693_NBBYTE_UID-1]!=0xE0 || UIDout[ISO15693_NBBYTE_UID-2]!=M24LRXX_ICCODE)
		return ERRORCODE_GENERIC;
		

	FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
												ISO15693_REQFLAG_HIGHDATARATE,
												ISO15693_REQFLAG_INVENTORYFLAGNOTSET,
												ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
												ISO15693_REQFLAG_NOTSELECTED,
												ISO15693_REQFLAG_ADDRESSED,
												ISO15693_REQFLAG_OPTIONFLAGNOTSET,
												ISO15693_REQFLAG_RFUNOTSET);	

	//int8_t ISO15693_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
	// Uid and CRC = 0x00
	errchk(M24LRXX_GetSystemInfo (FlagsByteData,
							UIDout,
							ISO15693_APPENDCRC,
							0x00,
							ReaderRecBuf));	
				
//	SplitReaderReply (	SEND_RECEIVE,
//						PROTOCOL_TAG_ISO15693,
//						ReaderRecBuf,
//						&ResultCode,
//						&NbTagReplyByte,
//						TagReplyBuf,
//						&NbControlByte,
//						ControlBuf);

	errchk(M24LRXX_SplitGetSystemInfoResponse(	ReaderRecBuf,
											ReaderRecBuf[CR95HF_LENGTH_OFFSET],
											&Flags ,
											InfoFlags,
											UIDout,
											DSFIDout,
											AFIout,
											MemSizeOut,
											IcRefOut,
											&errorcode));
	
	return RESULTOK;

Error:
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function returns the tag AFI word
* @param  	AFIout		: 	tag AFI read
* @retval status function
*/
int8_t M24LRXX_GetAFI (uint8_t *AFIout) 
{ 
	return ISO15693_GetAFI (AFIout);
}

/**  
* @brief  this function returns the tag AFI word
* @param  	AFIout		: 	tag AFI read
* @retval status function
*/
int8_t M24LRXX_GetDSFID (uint8_t *DSFIDout) 
{ 
	return ISO15693_GetDSFID (DSFIDout);
}

/**  
* @brief  this function returns AFI flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  
* @retval 	DSFID flag
*/
int8_t M24LRXX_GetAFIFlag (uc8 FlagsByte)
{
	return (ISO15693_GetAFIFlag (FlagsByte));
}

/**  
* @brief  this function returns Tag memory size flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	tag memory size flag
*/
int8_t M24LRXX_GetMemorySizeFlag (uc8 FlagsByte)
{

	return (ISO15693_GetMemorySizeFlag (FlagsByte));
}

/**  
* @brief  this function returns Icc reference flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  

* @retval 	IC reference flag
*/
int8_t M24LRXX_GetICReferenceFlag (uc8 FlagsByte)
{
	return (ISO15693_GetICReferenceFlag (FlagsByte));
}


/**
 * @}
 */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
