/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso15693.c
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 31/05/2010
* Description        : Send command 15693 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "stm32f10x.h"
#include "lib_iso15693_LRIS2k.h"
#include "lib_iso15693.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
#include "main.h"

/** @addtogroup Libraries
 * 	@{
 */

/** @addtogroup ISOlibraries
 * 	@{
 */

 /** @addtogroup DualM24LR64
 * 	@{
 */


/** @defgroup ISO15693_Private_Functions
 *  @{
 */

//static int8_t M24LR64_IsCorrectCRC16Residue (uc8 *DataIn,uc8 Length);
//static int16_t M24LR64_CRC16 (uc8 *DataIn,uc8 NbByte);
static int8_t LRIS2K_IsFastReaderConfig ( uc8 ParameterSelected);
static int8_t LRIS2K_SplitBasicResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
static int8_t LRIS2K_IsReaderConfigMatchWithFlag( uc8 ParameterSelected , uc8 Flags);
static int8_t LRIS2K_IsReaderConfigMatchWithFlagWriteCmd( uc8 ParameterSelected , uc8 Flags);

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
int8_t LRIS2K_SelectProtocol	(uc8 DataRate, uc8 TimeOrSOF, uc8 Modulation, uc8 SubCarrier, uc8 AppendCRC)
{
	return ISO15693_ProtocolSelectProtocol	(DataRate, TimeOrSOF, Modulation, SubCarrier, AppendCRC);
}


/**  
* @brief  this function send an inventory command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	tag reply
* @retval status function
*/
int8_t LRIS2K_Inventory( uc8 Flags , uc8 AFIin, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  )
{
uint8_t 	LRIS2K_Flags;
	
	// force inventory flag to 1
	LRIS2K_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	return ISO15693_Inventory(	LRIS2K_Flags ,
							AFIin,
							MaskLength,
							MaskValue,
							AppendCRC,
							CRC16,
							pResponse  );

}

/**  
* @brief  this function send an inventory command  with the NbSlot flag force to One.
	it returns RESULTOK if the command was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_InventoryOneSlot( uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  )
{
int8_t 	NewFlags;
	
	// force NbSlot Flag to 1;
	NewFlags = Flags | ISO15693_MASK_ADDRORNBSLOTSFLAG;
	 
	return LRIS2K_Inventory( 	NewFlags ,
									AFI, 
									MaskLength, 
									MaskValue, 
									AppendCRC, 
									CRC16, 
									pResponse  );
}

/**  
* @brief  this function send an inventory command  with the NbSlot flag force to zero.
	it returns RESULTOK if the command was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	NbTag		:	Number of tag seen 
* @param	pResponse	:  	pointer of NbTag
* @retval 	Slot occupied (bit 0 = slot 0;bit 1 = slot 1;... )
*/
int16_t LRIS2K_Inventory16Slots( uc8 Flags , uc8 AFIin, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *NbTag, uint8_t *pResponse  )
{
	
return (ISO15693_Inventory16Slots( Flags , AFIin, MaskLength, MaskValue, AppendCRC, CRC16, NbTag, pResponse  ));

}

/**  
* @brief  this function send an EOF pulse
* @param  	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_SendEOF(uint8_t *pResponse  )
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
static int8_t LRIS2K_IsReaderConfigMatchWithFlag( uc8 ParameterSelected , uc8 Flags)
{
int8_t status;

	errchk(ISO15693_IsReaderConfigMatchWithFlag( ParameterSelected , Flags));

	
	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function returns RESULTOK if request flag is coherent with parameter of
	the select command (reader command). the subcarrier and data rate shall be 
	egual.	
* @param  	Flags					:  	inventory flags
			GloParameterSelected	:	
* @retval status function
*/
static int8_t LRIS2K_IsFastReaderConfig ( uc8 ParameterSelected)
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
static int8_t LRIS2K_IsReaderConfigMatchWithFlagWriteCmd( uc8 ParameterSelected , uc8 Flags)
{
int8_t status;

	errchk ( ISO15693_IsReaderConfigMatchWithFlagWriteCmd( ParameterSelected , Flags));

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitInventoryResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitFastInventoryInitiatedResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitInventoryInitiatedResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitFastInitiateResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits inventory response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitInitiateResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout)
{
	
	return ISO15693_SplitInventoryResponse( TagResponse,Length,Flags , DSFIDextract, UIDout);
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t LRIS2K_SplitGetSystemInfoResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *InfoFlags,uint8_t *UIDout,uint8_t *DSFIDout,uint8_t *AFIout,uint8_t *MemSizeOut,uint8_t *IcRefOut,uint8_t *ErrorCode)
{

	return ISO15693_SplitGetSystemInfoResponse(	TagResponse,
													Length,
													Flags ,
													InfoFlags,
													UIDout,
													DSFIDout,
													AFIout,
													MemSizeOut,
													IcRefOut,
													ErrorCode);

	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
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
int8_t LRIS2K_SplitReadSingleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadSingleBlockResponse( TagResponse,Length,NbByteOfData,OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
	
}

/**  
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
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
int8_t LRIS2K_FastSplitReadSingleBlockResponse( uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode)
{

	return ISO15693_SplitReadSingleBlockResponse( TagResponse,Length,NbByteOfData,OptionFlag,Flags ,SecurityStatus,Data,ErrorCode);
	
}



/**  
* @brief  this function splits GetMultiple Block Secutity status response. 
	If the residue of tag response	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
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
int8_t LRIS2K_SplitGetMultipleBlockSecutityResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uint8_t *Flags ,uint8_t *DataOut,uint8_t *ErrorCode)
{

	return ISO15693_SplitGetMultipleBlockSecutityResponse( TagResponse, Length, NbBlock,Flags ,DataOut,ErrorCode);

}

/**  * @brief  this function splits Write single block response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitWriteSingleBlockResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitWriteSingleBlockResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Write single block response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitWritePasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return LRIS2K_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}



/**  * @brief  this function splits Write AFI response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitWriteAFIResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitWriteAFIResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Write DSFID response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitWriteDSFIDResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitWriteDSFIDResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits reset To Ready response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitResetToReadyResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitResetToReadyResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Select response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitSelectResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitSelectResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Lcok block response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitLockBlockResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitLockBlockResponse( TagResponse,Length,Flags ,ErrorCode);

}

/**  * @brief  this function splits Lcok block response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitLockPasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{

	return ISO15693_SplitLockBlockResponse( TagResponse,Length,Flags ,ErrorCode);

}


/**  * @brief  this function splits Lcok AFI response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitLockAFIResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitLockAFIResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Lcok AFI response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitLockDSFIDResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitLockDSFIDResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits Present Password response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
int8_t LRIS2K_SplitPresentPasswordResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return LRIS2K_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}

/**  * @brief  this function splits tag response, which is composed with flags and CRC16 
(if error flag is not set) or with Flags and error code (if error flag is set).
If the residue of tag response is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	Flags		:  	response flag (1 byte)
* @param	ErrorCode 	: 	Error code if error flg is set
* @retval status function
*/
static int8_t LRIS2K_SplitBasicResponse( uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode)
{
	
	return ISO15693_SplitBasicResponse( TagResponse,Length,Flags ,ErrorCode);
	
}


/**  
* @brief  this function send an stay_quiet command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_StayQuiet(uc8 Flags,uc8 *UIDin,uc8 AppendCRC,uc8 *CRC16)
{	
	return ISO15693_StayQuiet(Flags,UIDin,AppendCRC,CRC16);	 
}		

/**  
* @brief  this function send an ReadSingleBlock command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_ReadSingleBlock (uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
	return ISO15693_ReadSingleBlock ( Flags, UIDin, BlockNumber,AppendCRC, CRC16,pResponse );	
}


/**  
* @brief  this function send a WriteSingleblock command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	: 
* @param	BlockLength :	Nb of byte of block length
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_WriteSingleBlock(uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 BlockLength,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{

	return ISO15693_WriteSingleBlock(Flags,UIDin,BlockNumber,BlockLength,DataToWrite,AppendCRC, CRC16,pResponse );
}


/**  
* @brief  this function send an WriteSingleblock command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	: 
* @param	BlockLength :	Nb of byte of block length
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_WritePassword(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[MAX_BUFFER_SIZE],
		NthByte=0;
int8_t	status;

	errchk (ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (ISO15693_IsReaderConfigMatchWithFlagWriteCmd (GloParameterSelected,Flags));

	if (PasswordNumber > LRIS2K_RFPASSWORD_MAX)
		return ERRORCODE_GENERIC;
	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_WRITEPASSWORD;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	DataToSend[NthByte++] = PasswordNumber;

	memcpy(&(DataToSend[NthByte]),DataToWrite,LRIS2K_BLOCK_NBBYTE);
	NthByte +=LRIS2K_BLOCK_NBBYTE;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}




/**  
* @brief  this function send a Present Sector Password command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	PasswordNumber	:	this value si betwenn 1 and 3 
* @param	Password	 :	32 bits word
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_PresentPassword(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_PRESENTSECTORPWD],
		NthByte=0;
int8_t	status;

	errchk (CHECKVAL(PasswordNumber,LRIS2K_RFPASSWORD_MIN,LRIS2K_RFPASSWORD_MAX));
	errchk (LRIS2K_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));

	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_PRESENTSECTORPWD;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}
	
	DataToSend[NthByte++] =PasswordNumber;

	memcpy(&(DataToSend[NthByte]),Password,LRIS2K_RFPASSWORD_NBBYTE);
	NthByte +=LRIS2K_RFPASSWORD_NBBYTE;


	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse)== ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send a Fast Read Single Block command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID	(optional)
* @param	BlockNumber	:	this value is between 1 and 2047
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_FastReadSingleBlock (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_FASTREADSINGLEBLOCK],
		NthByte=0,
		LRIS2K_Flags;
int8_t	status;

	// the protocol extension flag shall be set to 1
	LRIS2K_Flags = Flags | ISO15693_MASK_PROTEXTFLAG;

	errchk (LRIS2K_IsReaderConfigMatchWithFlag (GloParameterSelected,LRIS2K_Flags));
	errchk (LRIS2K_IsFastReaderConfig (GloParameterSelected));

	
	if (Uint8ToUint16 (BlockNumber)>LRIS2K_BLOCK_MAX)
		return ERRORCODE_GENERIC;
  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = LRIS2K_Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_FASTREADSINGLEBLOCK;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
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
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:  	AfI	(optional)
* @param	MaskLength	:	this value si betwenn 1 and 64 
* @param	Mask	 	:	64 bits word
* @param	AppendCRC	:	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_FastInventoryInitiated (uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_FASTINVENTORYINITIATED],
		NthByte=0,
		NbMaskBytes = 0,
		NbSignificantBits=0,
		LRIS2K_Flags;
int8_t	FirstByteMask,
		NthMaskByte = 0,
		status;
	
	// the inventory flag shall be set to 1
	LRIS2K_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	errchk(ISO15693_GetInventoryFlag (LRIS2K_Flags));
	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags))	
	
	DataToSend[NthByte++] = LRIS2K_Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_FASTINVENTORYINITIATED;
	DataToSend[NthByte++] = LRIS2K_ICCODE;
	
	if (ISO15693_GetSelectOrAFIFlag (LRIS2K_Flags) == TRUE)
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

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
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
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AFIin		:  	AfI	(optional)
* @param	MaskLength	:	this value si betwenn 1 and 64 
* @param	Mask	 	:	64 bits word
* @param	AppendCRC	:	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_InventoryInitiated (uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_INVENTORYINITIATED],
		NthByte=0,
		NbMaskBytes = 0,
		NbSignificantBits=0,
		LRIS2K_Flags;
int8_t	FirstByteMask,
		NthMaskByte = 0,
		status;
	
	// the inventory flag shall be set to 1
	LRIS2K_Flags = Flags | ISO15693_MASK_INVENTORYFLAG;

	errchk(ISO15693_GetInventoryFlag (LRIS2K_Flags));
	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags))	
	
	DataToSend[NthByte++] = LRIS2K_Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_INVENTORYINITIATED;
	DataToSend[NthByte++] = LRIS2K_ICCODE;
	
	if (ISO15693_GetSelectOrAFIFlag (LRIS2K_Flags) == TRUE)
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

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
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
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_FastInitiate (uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_FASTINITIATE],
		NthByte=0;
int8_t	status;


	errchk (LRIS2K_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (LRIS2K_IsFastReaderConfig (GloParameterSelected));

  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_FASTINITIATE;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

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
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_Initiate (uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{
uint8_t DataToSend[LRIS2K_MAXLENGTH_FASTINITIATE],
		NthByte=0;
int8_t	status;


	errchk (LRIS2K_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
	errchk (LRIS2K_IsFastReaderConfig (GloParameterSelected));

  	
	// block number is 16 bits word	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_INITIATE;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

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
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID (optional)
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_WriteAFI(uc8 Flags, uc8 *UIDin,uc8 AFIToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{

	return ISO15693_WriteAFI(Flags, UIDin,AFIToWrite,AppendCRC, CRC16,pResponse );
}

/**  
* @brief  this function send a write DSFID command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID (optional)
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_WriteDSFID(uc8 Flags, uc8 *UIDin,uc8 DSFIDToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse )
{

	return ISO15693_WriteDSFID(Flags, UIDin,DSFIDToWrite,AppendCRC, CRC16,pResponse );
}


/**  
* @brief  this function send an select command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_Select ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
	return ISO15693_Select ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}	

/**  
* @brief  this function send an select command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_ResetToReady ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{

	return ISO15693_ResetToReady ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}


/**  
* @brief  this function send an Lock AFI command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_LockAFI ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse )
{
int8_t 	status;
uint8_t LRIS2K_Flags;
	
	errchk(LRIS2K_IsReaderConfigMatchWithFlagWriteCmd( GloParameterSelected ,LRIS2K_Flags));

	return ISO15693_LockAFI ( LRIS2K_Flags, UIDin, AppendCRC,CRC16,pResponse );
Error:
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send an LockBlock command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	BlockNumber	: 
* @param	AppendCRC	: 	CRC16 can be write by either reader or soft or be a copy of input parameter
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_LockSingleBlock ( uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
  	return ISO15693_LockSingleBlock ( Flags, UIDin, BlockNumber,AppendCRC,CRC16,pResponse);
}


/**  
* @brief  this function send an LockBlock command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	BlockNumber	: 
* @param	AppendCRC	: 	CRC16 can be write by either reader or soft or be a copy of input parameter
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t LRIS2K_LockPassword ( uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 ProtectStatus,uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
uint8_t DataToSend[ISO15693_MAXLENGTH_LOCKSINGLEBLOCK],
		NthByte=0;
int8_t	status;

	errchk(LRIS2K_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));

	// if bit8 =1 then lock password command lock a password (0 to 3)
	if ((ProtectStatus& 0x80) ==1 && (ProtectStatus& 0x7C) !=0)
		return ERRORCODE_GENERIC;

	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = LRIS2K_CMDCODE_LOCKSECTORPWD;
	DataToSend[NthByte++] = LRIS2K_ICCODE;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	DataToSend[NthByte++] = BlockNumber;
	DataToSend[NthByte++] = ProtectStatus;

	if (AppendCRC == ISO15693_DONTAPPENDCRC)
	{	DataToSend[NthByte++] = CRC16[0];
		DataToSend[NthByte++] = CRC16[1];
	}

	errchk(CR95HF_SendRecv(NthByte,DataToSend,pResponse));
	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;


	return RESULTOK;
Error:
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
}

/**  
* @brief  this function send an Lock DSFID command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_LockDSFID ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
	return ISO15693_LockDSFID ( Flags, UIDin, AppendCRC,CRC16,pResponse);
}

/**  
* @brief  this function send an GetSystemInfo command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse)
{
uint8_t LRIS2K_Flags;

	return ISO15693_GetSystemInfo ( LRIS2K_Flags, UIDin, AppendCRC,CRC16,pResponse);
}


/**  
* @brief  this function send a Get Multiple Block Secutity Status  command and returns
 RESULTOK if the command was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	BlockNumber	: 
* @param	NbBlocks	: 	Number of blocks
* @param	AppendCRC	 :	if set to 1 the CRC will be somputing by the reader
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @param	pResponse	: 	Tag reply
* @retval status function
*/
int8_t LRIS2K_GetMultipleBlockSecutityStatus (uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 *NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse)
{
	return LRIS2K_GetMultipleBlockSecutityStatus (Flags, UIDin, BlockNumber, NbBlocks,AppendCRC, CRC16,pResponse);
}



///**  
//* @brief  this function computes the CRC16 as defined by CRC ISO/IEC 13239
//* @param  	DataIn		:	input to data 
//* @param	Length 		: 	Number of bits of DataIn
//* @param	ResCrc		: 	CRC16 computed
//* @retval status function
//*/
//int8_t LRIS2K_InventoryScanChain (uint8_t *UIDin,uint8_t AFIin, uint8_t *ScanChainResult)
//
//{
//	return ISO15693_InventoryScanChain (UIDin,AFIin, ScanChainResult);
//}

///**  
//* @brief  	this function computes the CRC16 as defined by CRC ISO/IEC 13239
//* @param  	UIDin			:	input to data 
//* @param	AFIin			: 	Number of bits of DataIn
//* @param	ScanChainResult	: 	CRC16 computed
//* @retval status function
//*/
//int8_t LRIS2K_ReadWriteSingleBlockScanChain (uint8_t *UIDin,uint8_t AFIin, uint8_t *ScanChainResult)
//
//{
//uint8_t FlagsByteData,
//		BlockNumber = 0,
//	//	TagReply	[MAX_BUFFER_SIZE],
//		TagReplyBuf	[MAX_BUFFER_SIZE],
//		ControlBuf [CONTROL_MAX_NBBYTE],
//		OptionFlag,
//		BlockSize,
//		NbBlock[LRIS2K_NBBYTE_NBBLOCK],
//		NthByte,
//		NbControlByte,
//		NbTagReplyByte,
//		status,
//		NthOptionFlag,
//		NthAddreFlag,
//		DataToWrite[ISO15693_MAXLENGTH_BLOCKSIZE],
//		DataRead [ISO15693_MAXLENGTH_BLOCKSIZE],
//		AddressFlag,
//		ResultCode,
//		Flags[ISO15693_NBBYTE_REPLYFLAG],		
//		SecurityStatus [ISO15693_NBBYTE_BLOCKSECURITY],
//		ErrorCode,
//		SelectFlag,
//		NthTest=0;
////uint16_t	NbBlockInt =0;
//
//	
//	//Reader_FieldOff();
////	delay_ms(5);
//	 *ScanChainResult = 0;
//	 // get the number of byte of a memoty block and the number of block
//	errchk(LRIS2K_GetMemSizeInfo (NbBlock,&BlockSize));
////	NbBlockInt = Uint8ToUint16(  NbBlock);
//	
//	if (BlockSize > ISO15693_MAXLENGTH_BLOCKSIZE)
//		return ERRORCODE_GENERIC;
//
//	
//	for (NthOptionFlag=0;NthOptionFlag<2;NthOptionFlag++)
//	{	
//		if (NthOptionFlag==0)
//		{	OptionFlag	=ISO15693_REQFLAG_OPTIONFLAGNOTSET;
//			SelectFlag = ISO15693_RESPECT_312;
//		}
//		else 
//		{	OptionFlag=ISO15693_REQFLAG_OPTIONFLAGSET;
//			SelectFlag = ISO15693_WAIT_FOR_SOF;
//
//		}
//
//		for (NthAddreFlag=0;NthAddreFlag<2;NthAddreFlag++)
//		{	
//			if (NthAddreFlag==0)
//				AddressFlag=ISO15693_REQFLAG_NOTADDRESSES;
//			else 
//				AddressFlag=ISO15693_REQFLAG_ADDRESSED;
//		
//			FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
//															ISO15693_REQFLAG_HIGHDATARATE,
//															ISO15693_REQFLAG_INVENTORYFLAGNOTSET,
//															ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
//															ISO15693_REQFLAG_NOTSELECTED,
//															AddressFlag,
//															OptionFlag,
//															ISO15693_REQFLAG_RFUNOTSET);
//			
//			status = ISO15693_SelectProtocol	(
//										ISO15693_TRANSMISSION_26,
//										SelectFlag,
//										ISO15693_MODULATION_100,
//										ISO15693_SINGLE_SUBCARRIER,
//										ISO15693_APPENDCRC);
//		
//		
//		
//			// read single block
//			ISO15693_ReadSingleBlock (	FlagsByteData, 
//										UIDin,
//										BlockNumber,
//										ISO15693_APPENDCRC,
//										0x00,
//										ReaderRecBuf );
//			
//			SplitReaderReply (	SEND_RECEIVE,
//								PROTOCOL_TAG_ISO15693,
//								ReaderRecBuf,
//								&ResultCode,
//								&NbTagReplyByte,
//								TagReplyBuf,
//								&NbControlByte,
//								ControlBuf);
//			
//			
//			if (ResultCode==RESULTOK)
//			{		
//				OptionFlag = ISO15693_GetOptionFlag (FlagsByteData);
//				ISO15693_SplitReadSingleBlockResponse( 	TagReplyBuf,
//														NbTagReplyByte,
//														BlockSize,
//														OptionFlag,
//														Flags ,
//														SecurityStatus,
//														DataRead,
//														&ErrorCode);
//		
//			}
//			
//			// create the data to write					  
//			for (NthByte = 0;NthByte <BlockSize;NthByte++)
//				DataToWrite[NthByte] = ~DataRead[NthByte];
//				
//		
//			status = ISO15693_WriteSingleBlock(FlagsByteData, 
//										UIDin,
//										BlockNumber,
//										BlockSize,
//										DataToWrite,
//										ISO15693_APPENDCRC,
//										0x00,
//										ReaderRecBuf );
//
//			if (OptionFlag==ISO15693_REQFLAG_OPTIONFLAGSET)
//			{
//				delay_ms(20);
//				ISO15693_SendEOF(ReaderRecBuf );	
//			}
//		
//		
//			// read single block
//			ISO15693_ReadSingleBlock (	FlagsByteData, 
//										UIDin,
//										BlockNumber,
//										ISO15693_APPENDCRC,
//										0x00,
//										ReaderRecBuf );
//			
//			SplitReaderReply (	SEND_RECEIVE,
//								PROTOCOL_TAG_ISO15693,
//								ReaderRecBuf,
//								&ResultCode,
//								&NbTagReplyByte,
//								TagReplyBuf,
//								&NbControlByte,
//								ControlBuf);
//			
//			
//			if (ResultCode==RESULTOK)
//			{		
//				OptionFlag = ISO15693_GetOptionFlag (FlagsByteData);
//				ISO15693_SplitReadSingleBlockResponse( 	TagReplyBuf,
//														NbTagReplyByte,
//														BlockSize,
//														OptionFlag,
//														Flags ,
//														SecurityStatus,
//														DataRead,
//														&ErrorCode);
//		
//			}
//			// check the data read 	
//			for (NthByte = 0;NthByte <BlockSize;NthByte++)
//			{	if (DataRead[NthByte] != DataToWrite[NthByte])
//					status = ERRORCODE_GENERIC;
//			}
//			if (status==RESULTOK)
//				*ScanChainResult = *ScanChainResult | (1 << NthTest);
//
//			NthTest ++ ;
//
//		}
//	}
//	
//
//	return RESULTOK;
// Error:
//	return ERRORCODE_GENERIC;
//}

/**  
* @brief  this function return a tag UID 
* @param  	UIDout: 	UID of a tag in the field
* @retval status function
*/
int8_t LRIS2K_GetUID (uint8_t *UIDout) 
{ 
	
	return ISO15693_GetUID (UIDout);

	
}


/**  
* @brief  this function return a tag UID 
* @param  	NbBlock		: 	number of block of tag memory 
* @param	BlockSize	:	number of byte of a memory block
* @retval status function
*/
int8_t LRIS2K_GetMemSizeInfo (uint8_t *NbBlock,uint8_t *BlockSize) 
{ 
	return ISO15693_GetMemSizeInfo (NbBlock,BlockSize); 

}

/**  
* @brief  this function returns true if the tag is a M24LR64. the two last UID bytes shall be E0 02 (02 means 
	STM product) and the IC ref returned by get system info shall be 0x2C
		
* @param  	Mask		: 	mask used for inventory (optional)
* @param  	MaskLength	: 	mask length (nb of bit)
* @retval status function
*/
int8_t LRIS2K_IsALRIS2K (uint8_t *Mask,uint8_t MaskLength) 
{ 
uint8_t	IcRefOut,
		UIDout[ISO15693_NBBYTE_UID];
int8_t	status;

	if ( MaskLength > ISO15693_NBBYTE_UID * 8)
		return ERRORCODE_GENERIC;

	
	errchk(LRIS2K_GetUID (UIDout));


	if (UIDout[ISO15693_NBBYTE_UID-1]!=0xE0 || UIDout[ISO15693_NBBYTE_UID-2]!=LRIS2K_ICCODE)
		return ERRORCODE_GENERIC;
		
	errchk(LRIS2K_GetIcRef (&IcRefOut));

	if ( (IcRefOut & LRIS2K_ICREF_MASK) == LRIS2K_ICREF_VALUE)
		return RESULTOK	;

	
	return ERRORCODE_GENERIC;

Error:
	return LRIS2K_ERROR_CODE;
}

/**  
* @brief  this function returns true if the tag is a M24LR64. the two last UID bytes shall be E0 02 (02 means 
	STM product) and the IC ref returned by get system info shall be 0x2C
		
* @param  	Mask		: 	mask used for inventory (optional)
* @param  	MaskLength	: 	mask length (nb of bit)
* @retval status function
*/
int8_t LRIS2K_GetIcRef (uint8_t *IcRefOut) 
{ 

	return ISO15693_GetIcRef (IcRefOut) ;
}


/**  
* @brief  this function returns the tag AFI word
* @param  	AFIout		: 	tag AFI read
* @retval status function
*/
int8_t LRIS2K_GetAFI (uint8_t *AFIout) 
{ 

	return ISO15693_GetAFI (AFIout);
}

/**  
* @brief  this function returns the tag AFI word
* @param  	AFIout		: 	tag AFI read
* @retval status function
*/
int8_t LRIS2K_GetDSFID (uint8_t *DSFIDout) 
{ 
	return ISO15693_GetDSFID (DSFIDout);

}
//
///**  
//* @brief  	this function runs an anticollision sequence and returns the number of tag seen and their UID
//* @param  	NbTag		: 	Number of tag seen
//* @param	pUIDout		: 	pointer on tag UID
//* @retval 	status function
//*/
//int8_t LRIS2K_RunAntiCollision (uc8 Flags , uc8 AFI,uint8_t *NbTag,uint8_t *pUIDout)
//{
//	return ISO15693_RunAntiCollision (Flags , AFI,NbTag,pUIDout);
//}

