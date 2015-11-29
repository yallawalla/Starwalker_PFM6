/**
  ******************************************************************************
  * @file    lib_iso15693.c 
  * @author  MMY Application Team
  * @version V1.1
  * @date    15/07/2012
  * @brief   This file provides set of function defined into ISO15693 specification
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  */ 

#include "stm32f10x.h"
#include "lib_iso15693.h"
#include "lib_CR95HF.h"
#include "miscellaneous.h"
#include "hw_config.h"


/** @addtogroup library
 * 	@{
 */

/** @addtogroup ISO_library
 * 	@{
 */

 /** @addtogroup ISO15693
 * 	@{
 */

 /** @addtogroup LibCR95HF_Extern_Variables
 *  @{
 */

 /** @ brief timer function */
extern void delay_ms(uint16_t delay);
/** @ brief memory allocation for CR95Hf response */
extern uint8_t 	ReaderRecBuf[MAX_BUFFER_SIZE+3]; 	// buffer for SPI ou UART reception
/** @ brief parameter of ProtocolSelect parameter */
//extern uint8_t GloParameterSelected;



/**
  * @}
  */ 



/** @addtogroup ISO15693_GETFunctions
 *  @{
 */



/**  
* @brief  	this function returns Select Or AFI flag  (depending on inventory flag)
* @param  	FlagsByte	: Request flags on one byte	
* @retval 	Select Or AFI
*/
int8_t ISO15693_GetSelectOrAFIFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_SELECTORAFIFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  	this function returns address Or Number of slots flag  (depending on inventory flag)
* @param  	FlagsByte	: Request flags on one byte	
* @retval 	address Or Number of slots
*/
int8_t ISO15693_GetAddressOrNbSlotsFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_ADDRORNBSLOTSFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}




/**  
* @brief  this function returns Sub carrier flag 
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval SubCarrier flag
*/
int8_t ISO15693_GetSubCarrierFlag (uc8 FlagsByte)
{
	if ((FlagsByte & ISO15693_MASK_SUBCARRIERFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns Data rate flag 
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval data rate flag
*/
int8_t ISO15693_GetDataRateFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_DATARATEFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}


/**  
* @brief  this function returns Option flag  (depending on inventory flag)
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	Option flag
*/
int8_t ISO15693_GetOptionFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_OPTIONFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns Option flag  (depending on inventory flag)
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	Option flag
*/
int8_t ISO15693_GetProtocolExtensionFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_PROTEXTFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}



/**  
* @brief  this function returns DSFID flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	DSFID flag
*/
int8_t ISO15693_GetDSFIDFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_DSFIDFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns AFI flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  
* @retval 	DSFID flag
*/
int8_t ISO15693_GetAFIFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_AFIFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns Tag memory size flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	tag memory size flag
*/
int8_t ISO15693_GetMemorySizeFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_MEMSIZEFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns Icc reference flag from information flag
* @param  	FlagsByte	: the byts cantaining the eight flags  

* @retval 	IC reference flag
*/
int8_t ISO15693_GetICReferenceFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_ICREFFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns error flag
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	Error flag
*/
int8_t ISO15693_GetErrorFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_ERRORFLAG) != 0x00)
		return TRUE ;
	else
		return FALSE ;
}

/**  
* @brief  this function returns RESULTOK if the error flag is reset, ERRORCODE_GENERIC otherwise
* @param  	Flag		:	Flag 
* @retval status function
*/
uint8_t ISO15693_IsTagErrorCode  (uc8 Flag)
{
	// Error flag = 1=> error detected
	if ( ISO15693_GetErrorFlag (Flag) == TRUE)
		return RESULTOK;
	// Error flag = 0 => no error
	else
		return ERRORCODE_GENERIC;		
}

/**
  * @}
 */ 

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
int8_t ISO15693_CreateRequestFlag (uc8 SubCarrierFlag,uc8 DataRateFlag,uc8 InventoryFlag,uc8 ProtExtFlag,uc8 SelectOrAFIFlag,uc8 AddrOrNbSlotFlag,uc8 OptionFlag,uc8 RFUFlag)
{
int32_t FlagsByteBuf=0;

		FlagsByteBuf = 	(SubCarrierFlag 	& 0x01)			|
						((DataRateFlag  	& 0x01)	<< 1)	|
						((InventoryFlag 	& 0x01) << 2)	|
						((ProtExtFlag		& 0x01)	<< 3)	|
						((SelectOrAFIFlag   & 0x01)	<< 4)	|
						((AddrOrNbSlotFlag  & 0x01)	<< 5)	|
						((OptionFlag  		& 0x01) << 6)	|
						((RFUFlag  			& 0x01) << 7);

	return (int8_t) FlagsByteBuf; 
}

 /**  
* @brief  this function selects 15693 protocol accoording to input parameters
* @param  	DataRate	:  	tag data rate ( 6 or 26 or 52k)
* @param 	TimeOrSOF	: 	wait for SOF or respect 312 µs delay
* @param	Modulation	: 	10 or 100 %modulation depth
* @param	SubCarrier	: 	single or double sub carrier
* @param	AppendCRC	: 	if = 1 the reader computes the CRC command
* @retval status function
*/
int8_t ISO15693_SelectProtocol	( void )
{
uint8_t 	ParametersByte=0,
					pResponse[PROTOCOLSELECT_LENGTH];
int8_t		status;
	
		
	//memset (pResponse,0xFF,PROTOCOLSELECT_LENGTH);
	
	

   	ParametersByte =  	((ISO15693_APPENDCRC << ISO15693_OFFSET_APPENDCRC ) 	&  ISO15693_MASK_APPENDCRC) |
												((ISO15693_SINGLE_SUBCARRIER << ISO15693_OFFSET_SUBCARRIER)	& ISO15693_MASK_SUBCARRIER)	|
												((ISO15693_MODULATION_100 << ISO15693_OFFSET_MODULATION) & ISO15693_MASK_MODULATION) |
												((ISO15693_WAIT_FOR_SOF <<  ISO15693_OFFSET_WAITORSOF ) & ISO15693_MASK_WAITORSOF) 	|
												((ISO15693_TRANSMISSION_26 <<   ISO15693_OFFSET_DATARATE  )	& ISO15693_MASK_DATARATE);
	
	errchk(CR95HF_ProtocolSelect(ISO15693_SELECTLENGTH,ISO15693_PROTOCOL,&(ParametersByte),pResponse));

	if (CR95HF_IsReaderResultCodeOk (PROTOCOL_SELECT,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	// save the parameter of protocol in order to check coherence with request flag
	//GloParameterSelected = ParametersByte;
	
	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}


/**  
* @brief  this function send an GetSystemInfo command and returns RESULTOK if the command 
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	inventory flags
* @param	UID			:  	Tag UID
* @param	AppendCRC	: 	CRC16 can be write by either reader or soft or be a copy of input parameter
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval status function
*/
int8_t ISO15693_GetSystemInfo ( uc8 Flags, uc8 *UIDin, uint8_t *pResponse)
{
uint8_t DataToSend[ISO15693_MAXLENGTH_GETSYSTEMINFO],
		NthByte=0;
int8_t	status;

//	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));
			
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_GETSYSINFO;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
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
* @brief  this function returns RESULTOK if request flag is coherent with parameter of
* @brief  the select command (reader command). the subcarrier and data rate shall be 
* @brief  egual.	
* @param  	Flags					:  	inventory flags
* @retval 	status function
*/
int8_t ISO15693_IsReaderConfigMatchWithFlag( uc8 ParameterSelected , uc8 Flags)
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
* @brief  this function splits GetsystemInfo response. If the residue of tag response
	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
		was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	TagResponse	:  	
* @param	Length		:	Number of byte of tag response
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDout		:  	UIDout of tag response
* @retval status function
*/
int8_t ISO15693_SplitGetSystemInfoResponse( uc8 *ReaderResponse,uc8 Length,uint8_t *Flags ,uint8_t *InfoFlags,uint8_t *UIDoutindex,uint8_t *DSFIDout,uint8_t *AFIout,uint8_t *MemSizeOut,uint8_t *IcRefOut,uint8_t *ErrorCode)
{ 
int8_t 	status,
		NbByte = ISO15693_GETSYSINFOOFFSET_DSFID;
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
	
	// the CR95HF returned an error code
	if (ResultCode == CR95HF_ERROR_CODE)
	{
		*ErrorCode = CR95HF_ERROR_CODE; 
		return CR95HF_ERROR_CODE;	
	}

	errchk(ISO15693_IsCorrectCRC16Residue (&(ReaderResponse[TagReplyIndex]),NbTagReplyByte));
	
	*Flags = ReaderResponse[TagReplyIndex+ISO15693_OFFSET_FLAGS];

	// the tag returned an errior code
	if (ISO15693_IsTagErrorCode  (*Flags) == RESULTOK)
	{	*ErrorCode =ReaderResponse[TagReplyIndex+NbByte++];
		return RESULTOK;		
	}	

	*InfoFlags = ReaderResponse[TagReplyIndex+ISO15693_GETSYSINFOOFFSET_INFOFLAGS];
	
	*UIDoutindex = 	TagReplyIndex+ISO15693_GETSYSINFOOFFSET_UID;
	
	if (ISO15693_GetDSFIDFlag (*InfoFlags) == TRUE)
		*DSFIDout = ReaderResponse[TagReplyIndex+NbByte++];
	if (ISO15693_GetAFIFlag (*InfoFlags) == TRUE)
		*AFIout = ReaderResponse[TagReplyIndex+NbByte++];
	if (ISO15693_GetMemorySizeFlag (*InfoFlags) == TRUE)
	{
		MemSizeOut[0] = ReaderResponse[TagReplyIndex+NbByte++]; 
		MemSizeOut[1] = ReaderResponse[TagReplyIndex+NbByte++]; 
	}
	if (ISO15693_GetICReferenceFlag (*InfoFlags) == TRUE)
		*IcRefOut = ReaderResponse[TagReplyIndex+NbByte++]; 
		
	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
	
}



 
/** @addtogroup ISO15693_CommandFunctions
*  @brief  this set of function send a command defined in ISO1593 specification
 *  @{
 */

/**  
* @brief  	this function send an inventory command to a contacless tag.  
* @param  	Flags		:  	Request flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	mask value which is compare to Contacless tag UID 
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ISO15693_ERRORCODE_PARAMETERLENGTH	: 	MaskLength value is erroneous
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_Inventory( uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uint8_t *pResponse  )
{
uint8_t 	NthByte = 0,
			InventoryBuf [ISO15693_MAXLENGTH_INVENTORY],
			NbMaskBytes = 0,
			NbSignificantBits=0;
int8_t 		FirstByteMask,
			NthMaskByte = 0,
			status;
	
	// initialize the result code to 0xFF and length to 0  in case of error 
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;

	if (MaskLength>ISO15693_NBBITS_MASKPARAMETER)
		return ISO15693_ERRORCODE_PARAMETERLENGTH;

	errchk(ISO15693_IsInventoryFlag (Flags));
		
	
	InventoryBuf[NthByte++] = Flags;
	InventoryBuf[NthByte++] = ISO15693_CMDCODE_INVENTORY;
	
	if (ISO15693_GetSelectOrAFIFlag (Flags) == TRUE)
		InventoryBuf[NthByte++] = AFI;
	
	InventoryBuf[NthByte++] = MaskLength;

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
				InventoryBuf[NthByte++] = MaskValue[NthMaskByte];
		}
	
		if (NbSignificantBits !=0)
			InventoryBuf[NthByte++] = MaskValue[NthMaskByte] & FirstByteMask;
	}

 	errchk(CR95HF_SendRecv(NthByte,InventoryBuf,pResponse));

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) != RESULTOK)
		return ERRORCODE_GENERIC;

	return RESULTOK;
Error:
	return ERRORCODE_GENERIC;
}

 /**  
* @brief  this function send an stay_quiet command to contacless tag.
* @param  	Flags		:  	Request flags
* @param	UIDin		:  	pointer on contactless tag UID
* @param	CRC16		: 	CRC16 of inventoty command (optional) (depends of protocol config)
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_StayQuiet(uc8 Flags,uc8 *UIDin)
{
uint8_t DataToSend[ISO15693_MAXLENGTH_STAYQUIET],
	 	NthByte = 0,
		pResponse[ISO15693_MAXLENGTH_REPLYSTAYQUIET];
int8_t	status;

	// the StayQuiet command shall always be executed in adrressed mode ( Select_flag is set 
	// to 0 and addreess flag is set to 1)
	errchk (ISO15693_IsAddressOrNbSlotsFlag (Flags));

	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_STAYQUIET;

	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
	NthByte +=ISO15693_NBBYTE_UID;	

   CR95HF_SendRecv(NthByte,DataToSend,pResponse);


	return RESULTOK;  
Error:	
	*pResponse = SENDRECV_ERRORCODE_SOFT;
	*(pResponse+1) = 0x00;
	return ERRORCODE_GENERIC;
	 
}	


/**  
* @brief  	this function send a ResetToReady command to contacless tag.
* @param  	Flags		:  	Request flags
* @param	UIDin		:  	pointer on contacless tag UID (optional) (depend on address flag of Request flags)
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_ResetToReady ( uc8 Flags, uc8 *UIDin, uint8_t *pResponse)
{
uint8_t DataToSend[ISO15693_MAXLENGTH_RESETTOREADY],
		NthByte=0;
int8_t  status;


	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_RESETTOREADY;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{
		memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
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
* @brief  this function send an ReadSingleBlock command to contactless tag.
* @param  	Flags		:  	Request flags
* @param	UIDin		:  	pointer on contacless tag UID (optional) (depend on address flag of Request flags)
* @param	BlockNumber	:  	index of block to read
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_ReadSingleBlock (uc8 Flags, uc8 *UIDin, uc16 BlockNumber,uint8_t *pResponse )
{
uint8_t DataToSend[ISO15693_MAXLENGTH_READSINGLEBLOCK],
		NthByte=0;
//int8_t 	status;

//	errchk(ISO15693_IsReaderConfigMatchWithFlag (GloParameterSelected,Flags));

	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_READSINGLEBLOCK;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	if (ISO15693_GetProtocolExtensionFlag (Flags) 	== FALSE)
		DataToSend[NthByte++] = BlockNumber;
	else 
	{
		DataToSend[NthByte++] = BlockNumber & 0x00FF;
		DataToSend[NthByte++] = (BlockNumber & 0xFF00 ) >> 8;	
	}

	CR95HF_SendRecv(NthByte,DataToSend,pResponse);

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;

// Error:
// 	*pResponse = SENDRECV_ERRORCODE_SOFT;
// 	*(pResponse+1) = 0x00;
// 	return ERRORCODE_GENERIC;
}


/**  
* @brief  	this function send an WriteSingleblock command and returns RESULTOK if the command 
* @brief  	was correctly emmiting, ERRORCODE_GENERIC otherwise
* @param  	Flags		:  	Request flags
* @param	UIDin		:  	pointer on contacless tag UID (optional) (depend on address flag of Request flags)
* @param	BlockNumber	:  	index of block to write
* @param	BlockLength :	Nb of byte of block length
* @param	DataToWrite :	Data to write into contacless tag memory
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_WriteSingleBlock(uc8 Flags, uc8 *UIDin, uc16 BlockNumber,uc8 *DataToWrite,uint8_t *pResponse )
{
uint8_t DataToSend[MAX_BUFFER_SIZE],
		NthByte=0,
		BlockLength = ISO15693_NBBYTE_BLOCKLENGTH;

	
	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_WRITESINGLEBLOCK;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	if (ISO15693_GetProtocolExtensionFlag (Flags) 	== FALSE)
		DataToSend[NthByte++] = BlockNumber;
	else 
	{
		DataToSend[NthByte++] = BlockNumber & 0x00FF;
		DataToSend[NthByte++] = (BlockNumber & 0xFF00 ) >> 8;
		
	}
	
	memcpy(&(DataToSend[NthByte]),DataToWrite,BlockLength);
	NthByte +=BlockLength;

	if (ISO15693_GetOptionFlag (Flags) == FALSE)
		CR95HF_SendRecv(NthByte,DataToSend,pResponse);
	else 
	{	CR95HF_SendRecv(NthByte,DataToSend,pResponse);
	 	delay_ms(20);
		ISO15693_SendEOF (pResponse);
	}	

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;
	
}

/**  
* @brief  this function send an ReadSingleBlock command to contactless tag.
* @param  	Flags		:  	Request flags
* @param	UIDin		:  	pointer on contacless tag UID (optional) (depend on address flag of Request flags)
* @param	BlockNumber	:  	index of block to read
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_ReadMultipleBlock (uc8 Flags, uc8 *UIDin, uc16 BlockNumber, uc8 NbBlock, uint8_t *pResponse )
{
uint8_t DataToSend[ISO15693_MAXLENGTH_READSINGLEBLOCK],
		NthByte=0;


	DataToSend[NthByte++] = Flags;
	DataToSend[NthByte++] = ISO15693_CMDCODE_READMULBLOCKS;

	if (ISO15693_GetAddressOrNbSlotsFlag (Flags) 	== TRUE)
	{	memcpy(&(DataToSend[NthByte]),UIDin,ISO15693_NBBYTE_UID);
		NthByte +=ISO15693_NBBYTE_UID;	
	}

	if (ISO15693_GetProtocolExtensionFlag (Flags) 	== FALSE)
		DataToSend[NthByte++] = BlockNumber;
	else 
	{
		DataToSend[NthByte++] = BlockNumber & 0x00FF;
		DataToSend[NthByte++] = (BlockNumber & 0xFF00 ) >> 8;
		
	}
	
	DataToSend[NthByte++] = NbBlock;

	CR95HF_SendRecv(NthByte,DataToSend,pResponse);

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) == ERRORCODE_GENERIC)
		return ERRORCODE_GENERIC;

	return RESULTOK;


}


/**  
* @brief  	this function send an inventory command to a contactless tag. 
* @brief  	The NbSlot flag of Request flags is set (1 value). 
* @param  	Flags		:  	Request flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	mask value which is compare to Contacless tag UID 
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ISO15693_ERRORCODE_PARAMETERLENGTH	: 	MaskLength value is erroneous
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_InventoryOneSlot( uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uint8_t *pResponse  )
{
int8_t 	NewFlags,
		status;
	
	// force NbSlot Flag to 1;
	NewFlags = Flags | ISO15693_MASK_ADDRORNBSLOTSFLAG;
	 
	status = ISO15693_Inventory( 
									NewFlags ,
									AFI, 
									MaskLength, 
									MaskValue,  
									pResponse  );

	return status;
}

/**  
* @brief  	this function send an inventory command to a contacless tag. 
* @brief  	The NbSlot flag of Request flags is reset (0 value). 
* @param  	Flags		:  	Request flags
* @param	AFI			:	AFI byte (optional)
* @param	MaskLength 	: 	Number of bits of mask value
* @param	MaskValue	:  	mask value which is compare to Contacless tag UID 
* @param	AppendCRC	:  	CRC16 management. If set CR95HF appends CRC16. 
* @param	CRC16		: 	pointer on CRC16 (optional) in case of user has choosed to manage CRC16 (see ProtocolSelect command CR95HF layer)
* @param	NbTag		: 	Nbtag inventoried
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ISO15693_ERRORCODE_PARAMETERLENGTH	: 	MaskLength value is erroneous
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int16_t ISO15693_Inventory16Slots( uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uint8_t *NbTag, uint8_t *pResponse  )
{
int8_t 		NthSlot = 0,
			status,
			NewFlags;
int16_t		ReturnValue=0;
uint8_t		pOneTagResponse [ISO15693_NBBYTE_UID+3+2];
	
	// force NbSlot Flag to 0;
	NewFlags = Flags & ~ISO15693_MASK_ADDRORNBSLOTSFLAG;

	*NbTag = 0;
	status = ISO15693_Inventory( 
									NewFlags ,
									AFI, 
									MaskLength, 
									MaskValue, 
									pOneTagResponse  );
	

	if (status == RESULTOK && ISO15693_IsCollisionDetected (pOneTagResponse)== RESULTOK)
			ReturnValue = 0x0001;
	else if (status == RESULTOK)
	{
		memcpy(pResponse,pOneTagResponse,pOneTagResponse[CR95HF_LENGTH_OFFSET]+2);
	   	(*NbTag)++;
		
	}
	
		
	while (NthSlot++ <15 )
	{
		delayHighPriority_ms(5);

		status = ISO15693_SendEOF(pOneTagResponse  );

		if (status == RESULTOK && ISO15693_IsCollisionDetected (pOneTagResponse)== RESULTOK)
			ReturnValue |= (1<<NthSlot);
		else if (status == RESULTOK)
		{	memcpy(&(pResponse[(*NbTag)*(pOneTagResponse[CR95HF_LENGTH_OFFSET]+2)]),pOneTagResponse,pOneTagResponse[CR95HF_LENGTH_OFFSET]+2);
			(*NbTag)++;
		}
		
		
	}

	return ReturnValue;

}

/**  
* @brief  	this function send an EOF pulse to contactless tag.
* @param	pResponse	: 	pointer on CR95HF response
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_SendEOF(uint8_t *pResponse  )
{
	
	CR95HF_SendEOF(pResponse);

	if (CR95HF_IsReaderResultCodeOk (SEND_RECEIVE,pResponse) != CR95HF_SUCCESS_CODE)
		return ERRORCODE_GENERIC;
		
	return RESULTOK;

}





/**
  * @}
 */ 

/** @addtogroup ISO15693_ISFunctions
 *  @{
 */


/**  
* @brief  	this function returns RESULTOK is Inventorye flag is set
* @param  	FlagsByte	: the byts cantaining the eight flags  	
* @retval 	Inventory flag
*/
int8_t ISO15693_IsInventoryFlag (uc8 FlagsByte)
{
	if ((FlagsByte & ISO15693_MASK_INVENTORYFLAG) != 0x00)
		return RESULTOK ;
	else
		return ERRORCODE_GENERIC ;
}

/**  
* @brief  	this function returns RESULTOK if address Or Number of slots flag is set (depending on inventory flag)
* @param  	FlagsByte	: Request flag  	
* @retval 	RESULTOK : address or Number of slots flag is set
* @retval 	ERRORCODE_GENERIC : address Or Number of slots flag is reset
*/
int8_t ISO15693_IsAddressOrNbSlotsFlag (uc8 FlagsByte)
{

	if ((FlagsByte & ISO15693_MASK_ADDRORNBSLOTSFLAG) != 0x00)
		return RESULTOK ;
	else
		return ERRORCODE_GENERIC ;
}


 /**  
* @brief  	this function returns RESULTOK if a tag has reply, ERRORCODE_GENERIC otherwise
* @param	pResponse	: 	pointer on CR95HF response	
* @retval 	RESULTOK	: 	CR95HF returns a succesful code
* @retval 	ERRORCODE_GENERIC	: 	 CR95HF returns an error code
*/
int8_t ISO15693_IsATagInTheField (uc8 *pResponse)
{
	
	if ( pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_RESULTSCODE_OK)
		return RESULTOK;
	// When a collision occurs between two or more tags the CR95HF response can be 88 00
	if ( pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_ERRORCODE_SOF)
		return RESULTOK;
	// When a collision occurs between two or more tags the CR95HF response can be 8E 00
	if ( pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_ERRORCODE_RECEPTIONLOST)
		return RESULTOK;

	return ERRORCODE_GENERIC;

}

/**  
* @brief  	this function returns RESULTOK if a collision has been detected, ERRORCODE_GENERIC otherwise
* @param	pResponse	: 	pointer on CR95HF response 	
* @retval 	RESULTOK	: 	collision detected
* @retval 	ERRORCODE_GENERIC	: 	 no collision detected
*/
int8_t ISO15693_IsCollisionDetected (uc8 *pResponse)
{
int8_t tmp=pResponse[CR95HF_LENGTH_OFFSET+pResponse[CR95HF_LENGTH_OFFSET]]& (CONTROL_15693_CRCMASK | CONTROL_15693_COLISIONMASK);

	// if reply status is SOF invalid, a collision might be occured
//	if (pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_ERRORCODE_SOF)
//		return RESULTOK;

 //  if (pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_ERRORCODE_RECEPTIONLOST)
//		return RESULTOK;

	// if reply status is Ok but the CRC is invalid, a collision might be occured
//	if (pResponse[READERREPLY_STATUSOFFSET] == SENDRECV_RESULTSCODE_OK &&
//		ISO15693_IsCorrectCRC16Residue (&(pResponse[CR95HF_DATA_OFFSET]),pResponse[CR95HF_LENGTH_OFFSET]-1)== ERRORCODE_GENERIC)
//		return RESULTOK;

	// if the two last bits of control byte is egual to one,  a collision occured
//	if ((pResponse[CR95HF_LENGTH_OFFSET+pResponse[CR95HF_LENGTH_OFFSET]]) & (CONTROL_15693_CRCMASK | CONTROL_15693_COLISIONMASK) == 0x03)	
 	if (tmp  == 0x03)	
		return RESULTOK;	
	
	return ERRORCODE_GENERIC;

}


 /**
  * @}
 */ 

/** @addtogroup ISO15693_SplitFunctions
 *  @{
 */
/**  
* @brief  	this function splits inventory response. If the residue of tag response	is incorrect the function returns ERRORCODE_GENERIC, otherwise RESULTOK
* @param  	ReaderResponse	:  	pointer on CR95HF response
* @param	Length		:	Number of byte of Reader Response
* @param  	Flags		:  	Response flags
* @param	DSFIDextract: 	DSPID of tag response
* @param	UIDoutIndex		:  	index of UIDout of tag response
* @retval 	RESULTOK : Contactless tag response validated			
* @retval 	CR95HF_ERROR_CODE : CR95HF teturned an error code
* @retval 	ISO15693_ERRORCODE_CRCRESIDUE : CRC16 residue is erroneous
*/
int8_t ISO15693_SplitInventoryResponse( uc8 *ReaderResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDoutIndex)
{
int8_t status;
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
	
	//  CR95HF returned an error code
	if (ResultCode  != CR95HF_SUCCESS_CODE)
		return CR95HF_ERROR_CODE;	
	
	errchk(ISO15693_IsCorrectCRC16Residue (&ReaderResponse[TagReplyIndex],NbTagReplyByte));
	
	*Flags = ReaderResponse[TagReplyIndex+ISO15693_OFFSET_FLAGS];
	*DSFIDextract =	ReaderResponse[TagReplyIndex+ISO15693_INVENTORYOFFSET_DSFID];

	*UIDoutIndex=TagReplyIndex+ISO15693_INVENTORYOFFSET_UID;
	
	return RESULTOK;
Error:
	return ISO15693_ERRORCODE_CRCRESIDUE;
	
}


/** @addtogroup ISO15693_CRC16Functions
 *  @{
 */

/**  
* @brief  	this function computes the CRC16 as defined by CRC ISO/IEC 13239
* @param  	DataIn		:	input data 
* @param	NbByte 		: 	Number of byte of DataIn
* @retval	ResCrc		: 	CRC16 computed
*/
int16_t ISO15693_CRC16 (uc8 *DataIn,uc8 NbByte) 
{ 
int8_t 	i,
		j; 
int32_t ResCrc = ISO15693_PRELOADCRC16;
	
	for (i=0;i<NbByte;i++) 
	{ 
		ResCrc=ResCrc ^ DataIn[i];
		for (j=8;j>0;j--) 
		{
			ResCrc = (ResCrc & ISO15693_MASKCRC16) ? (ResCrc>>1) ^ ISO15693_POLYCRC16 : (ResCrc>>1); 
		}
	} 

	return ((~ResCrc) & 0xFFFF);
	
} 

/**  
* @brief  	this function computes the CRC16 residue as defined by CRC ISO/IEC 13239
* @param  	DataIn		:	input to data 
* @param	Length 		: 	Number of bits of DataIn
* @retval 	RESULTOK  	:   CRC16 residue is correct	
* @retval 	ERRORCODE_GENERIC  	:  CRC16 residue is false
*/
int8_t ISO15693_IsCorrectCRC16Residue (uc8 *DataIn,uc8 Length)
{
int16_t ResCRC=0;

	// check the CRC16 Residue 
	if (Length !=0)
		ResCRC=ISO15693_CRC16 (DataIn, Length);
	
	if (((~ResCRC) & 0xFFFF) != ISO15693_RESIDUECRC16)
		return ERRORCODE_GENERIC;
	
	return RESULTOK;
}

 /**
  * @}
 */ 
/****************************************************************************
* @brief  	this function returns RESULTOK if a tag has reply, ERRORCODE_GENERIC otherwise
* @param  	
* @retval 	status function
***************************************************************************
*/
int8_t ISO15693_IsPresent (void)
{
uint8_t UIDout[ISO15693_NBBYTE_UID];
int8_t status ;

	status = ISO15693_GetUID (UIDout) ;	

	if (status == RESULTOK)
		return RESULTOK;
	else return ERRORCODE_GENERIC;

}

/**  
* @brief  this function return a tag UID 
* @param  	UIDout: 	UID of a tag in the field
* @retval status function
*/
int8_t ISO15693_GetUID (uint8_t *UIDout) 
{ 
int8_t 	FlagsByteData,
		status;
uint8_t	TagReply	[ISO15693_NBBYTE_UID+7];
uint8_t Tag_error_check;
	

//	memset(UIDout,0x00,ISO15693_NBBYTE_UID);
	
	
	// select 15693 protocol
	errchk(ISO15693_SelectProtocol	(	));

	
	
	FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
													ISO15693_REQFLAG_HIGHDATARATE,
													ISO15693_REQFLAG_INVENTORYFLAGSET,
													ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
													ISO15693_REQFLAG_NOTAFI,
													ISO15693_REQFLAG_1SLOT,
													ISO15693_REQFLAG_OPTIONFLAGNOTSET,
													ISO15693_REQFLAG_RFUNOTSET);
	
	errchk( status = ISO15693_Inventory (	FlagsByteData,
						 	0x00, 
						 	0x00,
							0x00,
							TagReply ));

	Tag_error_check = TagReply[OFFSET_LENGTH]+1;
	if((TagReply[Tag_error_check] & CRC_MASK) != 0x00 )
		return ERRORCODE_GENERIC;
	

	if (status == RESULTOK)
		memcpy(UIDout,&(TagReply[TAGREPPLY_OFFSET_UID]),ISO15693_NBBYTE_UID);


	return RESULTOK; 
Error:
	return ERRORCODE_GENERIC;	
}

/**  
* @brief  this function returns the tag AFI word
* @param  	AFIout		: 	tag AFI read
* @retval status function
*/
int8_t ISO15693_GetIcRef (uint8_t *IcRefOut) 
{ 
int8_t 	FlagsByteData;
uint8_t	TagReply	[ISO15693_MAXLENGTH_REPLYGETSYSTEMINFO+3],
  		Flags[ISO15693_NBBYTE_REPLYFLAG],
		InfoFlags[ISO15693_NBBYTE_INFOFLAG],
		AFIout[ISO15693_NBBYTE_AFI],
		MemSizeOut[ISO15693_NBBYTE_MEMORYSIZE],
		UIDout[ISO15693_NBBYTE_UID],
		DSFIDout,
		errorcode;
int8_t	status;

	*IcRefOut = 0x00;
	FlagsByteData = ISO15693_CreateRequestFlag 	(	ISO15693_REQFLAG_SINGLESUBCARRIER,
												ISO15693_REQFLAG_HIGHDATARATE,
												ISO15693_REQFLAG_INVENTORYFLAGNOTSET,
												ISO15693_REQFLAG_NOPROTOCOLEXTENSION,
												ISO15693_REQFLAG_NOTSELECTED,
												ISO15693_REQFLAG_NOTADDRESSED,
												ISO15693_REQFLAG_OPTIONFLAGNOTSET,
												ISO15693_REQFLAG_RFUNOTSET);
	// select 15693 protocol
	errchk(ISO15693_SelectProtocol	(	));

	// Uid and CRC = 0x00
	errchk(ISO15693_GetSystemInfo (FlagsByteData,
							0x00,
							TagReply));
	

	ISO15693_SplitGetSystemInfoResponse(	TagReply,
											TagReply[CR95HF_LENGTH_OFFSET],
											Flags ,
											InfoFlags,
											UIDout,
											&DSFIDout,
											AFIout,
											MemSizeOut,
											IcRefOut,
											&errorcode);


	return RESULTOK;

Error:
	return ERRORCODE_GENERIC;

}






/** @addtogroup ISO15693_AdvancedFunctions
 *  @{
 */



/**  
* @brief  	this function runs an anticollision sequence and returns the number of tag seen and their UID.
* @brief	The protocol select command has to be send first and the Flags parameterns shall be compliant
* @brief	with the parameters of the Protocol Select command.
* @param  	Flags		: 	request flags
* @param  	AFI			: 	AFI parameter (optional)
* @param  	NbTag		: 	Number of tag seen
* @param	pUIDout		: 	pointer on tag UID
* @retval 	RESULTOK	: 	function succesful executed  
* @retval 	ERRORCODE_GENERIC	: 	
*/
int8_t ISO15693_RunAntiCollision (uc8 Flags , uc8 AFI,uint8_t *NbTag,uint8_t *pUIDout)
{
int8_t		status;
uint8_t		MaskValue[ISO15693_NBBYTE_UID],
			MaskLength = 0,
			MaskStack [0x80*ISO15693_NBBYTE_UID];	//	can save 16 mask. the size of a mask is ISO15693_NBBYTE_UID.
												// the first byte is mask length (bit size). the orthers bytre are mask value
uint8_t		RequestFlags = Flags,
			offset=0,
			ReplyFlag,
			NbTagInventoried,
			DSFIDout,
			NthStackValue=1,
			i=0,
			Nbloop = 0,
			StayQuietFlags = (Flags & ~ISO15693_MASK_INVENTORYFLAG) | ISO15693_MASK_ADDRORNBSLOTSFLAG,
			UIDoutOffet = CR95HF_DATA_OFFSET + 2 ;
uint16_t	SlotOccupancy= 0x0000	;

	memset(MaskStack,0x00,16*ISO15693_NBBYTE_UID);
	*NbTag = 0;

	
	// checks if at least a tag is in the field 
	 ISO15693_InventoryOneSlot( 
								RequestFlags ,
								AFI,
								MaskLength,
								MaskValue,
								ReaderRecBuf);
		

	// no tag in the field
	if (ISO15693_IsATagInTheField (ReaderRecBuf) != RESULTOK)
		return RESULTOK;

	// check TagReply CRC residue and get tag UID
	status = ISO15693_SplitInventoryResponse(	ReaderRecBuf,
												ReaderRecBuf[CR95HF_LENGTH_OFFSET],
												&ReplyFlag ,
												&DSFIDout,
												&UIDoutOffet);
	
	// residue CRC ok (=> Only one tag in the field)
	if (status == RESULTOK) 
	{
		*NbTag = 1;	
		// send a stay quiet command to the inventoried tag 
		ISO15693_StayQuiet(StayQuietFlags ,&(ReaderRecBuf[UIDoutOffet]));
		// no collision => copy DSFID
		pUIDout[0] = DSFIDout ;
		// no collision => copy tag UID
		memcpy(&(pUIDout[1]),&(ReaderRecBuf[UIDoutOffet]),ISO15693_NBBYTE_UID);
		return RESULTOK;
	}

	delayHighPriority_ms(10);


	(*NbTag)=0;	

		
	// the folowing anticollsion sequence is preorder traversal algorithm 
	// the UIDs can be represnt as a binary tree
	do{

		memset(MaskValue,0x00,ISO15693_NBBYTE_UID);
		// unstack mask value
		NthStackValue -- ;
	   	memcpy(MaskValue,(MaskStack+NthStackValue*ISO15693_NBBYTE_UID+1),ISO15693_NBBYTE_UID);	


		// at this point : at least two tags are in the field
		// => run an inventory 16 slots

		// the ReaderRecBuf is MAX_BUFFER_SIZE + 3 (=256 +3  bytes)
		// if a response is available in eack slot => 16 * 15 (max size of inventory reply + 3 control bytes)
		//						=>	16 * 15  = 240 bytes < MAX_BUFFER_SIZE + 3
		SlotOccupancy =ISO15693_Inventory16Slots( 
								RequestFlags ,
								AFI,
								MaskLength,
								MaskValue,
								&NbTagInventoried,
								ReaderRecBuf);
		   	

					
		offset = 0;

		// loop on tag inventoried and copy the UID,
		for (i=0;i<NbTagInventoried;i++)
		{
			status = ISO15693_SplitInventoryResponse(&(ReaderRecBuf[offset]),
										ReaderRecBuf[CR95HF_LENGTH_OFFSET],
										&ReplyFlag ,
										&DSFIDout,
										&UIDoutOffet);
		
			if (status==RESULTOK)
			{
				// send a stay quiet command to the inventoried tag 
				ISO15693_StayQuiet(StayQuietFlags ,&(ReaderRecBuf[offset + UIDoutOffet]));
				// no collision => copy DSFID
				pUIDout[(*NbTag)*(ISO15693_NBBYTE_UID+1)] = DSFIDout ;
				// no collision => copy tag UID
				memcpy((pUIDout+(1+(*NbTag)*(ISO15693_NBBYTE_UID+1))),&(ReaderRecBuf[offset + UIDoutOffet]),ISO15693_NBBYTE_UID);
				(*NbTag)++ ;
			}
			offset += ReaderRecBuf[offset+CR95HF_LENGTH_OFFSET] + 2;
		} // for (i=0;i<NbTagInventoried;i++)
	
	   		
		// add 4 bits to the mask
		MaskLength +=4;
	 	// create and stack the masks for the next inventory commands
		for (i=0;i<16;i++)
		{
			/*
			A collision was detected in the inventory 16 slots
				=> add 4 bits to the mask
			*/
		
			if ((SlotOccupancy & (0x01 <<i)) != 0)
			{
				// create the mask which include the slot whare the collision occured
				MaskValue[(MaskLength-4)/8] |= (i << ((MaskLength-4)%8)) ;  
				// copy masklength and maskvalue into mask stack
				MaskStack[NthStackValue*ISO15693_NBBYTE_UID]= MaskLength;
				memcpy((MaskStack+(NthStackValue*ISO15693_NBBYTE_UID)+1),MaskValue,ISO15693_NBBYTE_UID);
				NthStackValue++;
			}
		}

		 			
		Nbloop ++;

	} while (NthStackValue >0 && Nbloop < 0x20);
	
	 return RESULTOK;

}


/**  
* @brief  	this function runs an anticollision sequence and returns the number of tag seen and their UID.
* @brief	The protocol select command has to be send first and the Flags parameterns shall be compliant
* @brief	with the parameters of the Protocol Select command.
* @param  	Flags		: 	request flags
* @param  	AFI			: 	AFI parameter (optional)
* @param  	NbTag		: 	Number of tag seen
* @param	pUIDout		: 	pointer on tag UID
* @retval 	RESULTOK	: 	function succesful executed  
* @retval 	ERRORCODE_GENERIC	: 	
*/
int8_t ISO15693_RunInventory16slots (uc8 Flags , uc8 AFI,uint8_t *NbTag,uint8_t *pUIDout)
{
int8_t		status;
uint8_t		MaskValue[ISO15693_NBBYTE_UID],
			MaskLength = 0,
			MaskStack [0x40*ISO15693_NBBYTE_UID];	//	can save 64 masks. the size of a mask is ISO15693_NBBYTE_UID.
												// the first byte is mask length (bit size). the orthers bytre are mask value
uint8_t		RequestFlags = Flags,
			offset=0,
			ReplyFlag,
			NbTagInventoried,
			DSFIDout,
 			i=0,
			UIDoutOffet = CR95HF_DATA_OFFSET + 2 ;


	memset(MaskStack,0x00,16*ISO15693_NBBYTE_UID);
	*NbTag = 0;


	ISO15693_Inventory16Slots( 
							RequestFlags ,
							AFI,
							MaskLength,
							MaskValue,
							&NbTagInventoried,
							ReaderRecBuf);
	
   	
	if (NbTagInventoried == 0)
	{	*NbTag = 0;		
		return RESULTOK;
	}

			
	offset = 0;
	// loop on tag inventoried and if there is no colission if the slot copy the UID,
	// otherwise get the slot where the  collision occured
	for (i=0;i<NbTagInventoried;i++)
	{
			
			status = ISO15693_SplitInventoryResponse(&(ReaderRecBuf[offset]),
										ReaderRecBuf[CR95HF_LENGTH_OFFSET],
										&ReplyFlag ,
										&DSFIDout,
										&UIDoutOffet);
		
			if (status==RESULTOK)
			{
				// no collision => copy DSFID
				pUIDout[(*NbTag)*(ISO15693_NBBYTE_UID+1)] = DSFIDout ;
				// no collision => copy tag UID
				memcpy((pUIDout+(1+(*NbTag)*(ISO15693_NBBYTE_UID+1))),&(ReaderRecBuf[UIDoutOffet+offset]),ISO15693_NBBYTE_UID);
				(*NbTag)++ ;
			}
//		}
		

		offset += ReaderRecBuf[offset+CR95HF_LENGTH_OFFSET] + 2;
	} // for (i=0;i<NbTagInventoried;i++)


	 return RESULTOK;

}

/**
* @brief  Find the TAG ISO15693 present in the field
* @param  *Length_Memory_TAG : return the memory size of the TAG
* @param  *Tag_Density : return 0 if the TAG is low density (< 16kbits), return 1 if the TAG is high density
* @param  *IC_Ref_TAG : return the IC_Ref of the TAG ( the 2 LSB bits are mask for the TAG LRiS2K, LRi2K and LRi1K)
* @retval ERRORCODE_GENERIC / RESULTOK.
*/
uint8_t ISO15693_GetTagIdentification (uint16_t *Length_Memory_TAG, uint8_t *Tag_Density, uint8_t *IC_Ref_TAG)
	{
uint8_t RepBuffer[32],
				RequestFlags = 0x0A;
//		uint8_t Get_Syst_Inf[2] = {0x0A, 0x2B};
//uint8_t Proto[1] = {0x09};
uint8_t IC_Ref;
		
		/*Use ISO15693 Protocol*/
		//CR95HF_ProtocolSelect(0x02,PROTOCOL_TAG_ISO15693,Proto,RepBuffer);
		ISO15693_SelectProtocol();
		
		
		/*Send Get_System_Info with Protocol Extention Flag Set*/
		if ( ISO15693_GetSystemInfo ( RequestFlags, 0x00, RepBuffer) == RESULTOK)
			IC_Ref = RepBuffer[17];
		else
		{
				RequestFlags = 0x02;
				if ( ISO15693_GetSystemInfo ( RequestFlags, 0x00, RepBuffer) == RESULTOK)
					IC_Ref = RepBuffer[16];
				else 
					return CR95HF_ERRORCODE_NOTAGFOUND; 	
		}
		
		switch (IC_Ref)
		{
			case ISO15693_M24LR64R :
				*Length_Memory_TAG = ((((RepBuffer[15] <<8)& 0xFF00) | RepBuffer[14])+1);
			  *Tag_Density = ISO15693_HIGH_DENSITY;
				break;
				
			case ISO15693_M24LR64ER :
				*Length_Memory_TAG = ((((RepBuffer[15] <<8)& 0xFF00) | RepBuffer[14])+1);
				*Tag_Density = ISO15693_HIGH_DENSITY;
				break;
				
			case ISO15693_M24LR16ER :
				*Length_Memory_TAG = ((((RepBuffer[15] <<8)& 0xFF00) | RepBuffer[14])+1);
				*Tag_Density = ISO15693_HIGH_DENSITY;
				break;
				
			case ISO15693_M24LR04ER :
				*Length_Memory_TAG = (RepBuffer[14]+1);
				*Tag_Density = ISO15693_LOW_DENSITY;
				break;
				
			case ISO15693_LRiS64K :
				*Length_Memory_TAG = ((((RepBuffer[15] <<8)& 0xFF00) | RepBuffer[14])+1);
				*Tag_Density = ISO15693_HIGH_DENSITY;
				break;

			default :
				/*Flag IC_REF LSB For LRiXX*/
				IC_Ref &= 0xFC;
			
				switch (IC_Ref)
				{		
					case ISO15693_LRiS2K :
						*Length_Memory_TAG = (RepBuffer[14]+1);
						*Tag_Density = ISO15693_LOW_DENSITY;
					break;
				
					case ISO15693_LRi2K :
						*Length_Memory_TAG = (RepBuffer[14]+1);
						*Tag_Density = ISO15693_LOW_DENSITY;
					break;
				
					case ISO15693_LRi1K :
						*Length_Memory_TAG = (RepBuffer[14]+1);
						*Tag_Density = ISO15693_LOW_DENSITY;
					break;
			
					default :
						return ERRORCODE_GENERIC;
				}
		}
		
		*IC_Ref_TAG = IC_Ref;
		return RESULTOK;
	}


#ifdef USE_MSD_DRIVE

/**
* @brief  Save the data from a TAG block for the uncomplete block to write
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  NbByte_To_Write : Numbre of Byte to write in the TAG
* @param  FirstByte_To_Write : First Byte to write in the TAG
* @param  *Data_To_Save : Data store before write the complete block of the TAG
* @param  *Length_Low_Limit : Number of Byte save for the first block to write (0 =< Length_Low_Limit =< 4)
* @param  *Length_High_Limit : Number of Byte save for the last block to write (0 =< Length_High_Limit =< 4)	
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	
uint8_t ISO15693_Tag_Save (uint8_t Tag_Density, uint16_t NbByte_To_Write, uint16_t FirstByte_To_Write, uint8_t *Data_To_Save, uint8_t *Length_Low_Limit, uint8_t *Length_High_Limit)
	{				 
	const uint8_t NbBytePerBlock = 0x04; 		 
	
	uint8_t RepBuffer[32],
					ReadSingleBuffer [4]={0x02, 0x20, 0x00, 0x00};

	uint8_t Nb_Byte_To_Save_Low;
	uint8_t Nb_Byte_To_Save_High;						
	uint16_t Num_Block_Low;
	uint16_t Num_Block_High;
					
					
/**********Find Limit Block*****************/ 
			 Num_Block_Low = FirstByte_To_Write / NbBytePerBlock;
			 Num_Block_High = (FirstByte_To_Write + NbByte_To_Write) / NbBytePerBlock;
					
			 Nb_Byte_To_Save_Low	= (FirstByte_To_Write) % NbBytePerBlock;
			 Nb_Byte_To_Save_High	= 4-((FirstByte_To_Write + NbByte_To_Write/*+ Nb_Byte_To_Save_Low*/) % NbBytePerBlock); 

/**********Read Low Limit******************/					 
				if(Tag_Density == ISO15693_LOW_DENSITY)		
				{
				ReadSingleBuffer[2] = Num_Block_Low;
				CR95HF_SendRecv (0x03,ReadSingleBuffer,RepBuffer);
				}
				
				
				else if(Tag_Density == ISO15693_HIGH_DENSITY)
				{
				 ReadSingleBuffer[0] = 0x0A;
				 ReadSingleBuffer[2] =  Num_Block_Low & 0x00FF;
				 ReadSingleBuffer[3] = (Num_Block_Low & 0xFF00) >> 8;
				 CR95HF_SendRecv (0x04,ReadSingleBuffer,RepBuffer);
				}		
				
				if(RepBuffer[0] != 0x80)
					return ERRORCODE_GENERIC;
				
				memcpy(&Data_To_Save[0], &RepBuffer[3], Nb_Byte_To_Save_Low);
				*Length_Low_Limit = Nb_Byte_To_Save_Low;
				
/**********Read High Limit******************/				 		
				if(Tag_Density == ISO15693_LOW_DENSITY)	
				{	
				ReadSingleBuffer[2] = Num_Block_High;
				CR95HF_SendRecv (0x03,ReadSingleBuffer,RepBuffer);
				}
				
				else if(Tag_Density == ISO15693_HIGH_DENSITY)
				{
				 ReadSingleBuffer[0] = 0x0A;
				 ReadSingleBuffer[2] =  Num_Block_High & 0x00FF;
				 ReadSingleBuffer[3] = (Num_Block_High & 0xFF00) >> 8;
				 CR95HF_SendRecv (0x04,ReadSingleBuffer,RepBuffer);	
				}
				
				if(RepBuffer[0] != 0x80)
					return ERRORCODE_GENERIC;
				
				/*Data temp*/
				memcpy(&Data_To_Save[4], &RepBuffer[3+(4-Nb_Byte_To_Save_High)], (Nb_Byte_To_Save_High));
				*Length_High_Limit = Nb_Byte_To_Save_High;
			return RESULTOK;
}	




/**
* @brief  Read Multiple Block in the TAG (sector size : 0x20 = 32 Blocks)
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  *Data_To_Read : return the data read in the TAG
* @param  NbBlock_To_Read : Number of block to read in the TAG
* @param  FirstBlock_To_Read : First block to read in the TAG
* @retval ERRORCODE_GENERIC / RESULTOK.
*/
uint8_t ISO15693_Read_Multiple_Tag_Data(uint8_t Tag_Density, uint8_t *Data_To_Read, uint16_t NbBlock_To_Read, uint16_t FirstBlock_To_Read)
{
uint8_t /*ReadMultipleBuffer [5]={0x02, 0x23, 0x00, 0x00, 0x00},*/
				RepBuffer[140],
				Requestflags = 0x02;

uint16_t NbSectorToRead = 0,
				SectorStart = 0,
				NthDataToRead,
				NumSectorToRead;
	
	/*Convert the block number in sector number*/
  NbSectorToRead = (NbBlock_To_Read/0x20)+1;	
	SectorStart = FirstBlock_To_Read/0x20;
	NumSectorToRead = SectorStart;

	// update the RequestFlags
	if (Tag_Density == ISO15693_LOW_DENSITY)
		NumSectorToRead =  (NumSectorToRead*0x20) & 0x00FF;
	else if (Tag_Density == ISO15693_HIGH_DENSITY)
		Requestflags = 0x0A;	
	else
		return ERRORCODE_GENERIC;
	
	for ( NthDataToRead=0; NthDataToRead < NbSectorToRead; NthDataToRead++)
	{			
			NumSectorToRead += NthDataToRead;

// 			if (Tag_Density == ISO15693_LOW_DENSITY)
// 			{
// 				ReadMultipleBuffer [2] = (NumSectorToRead*0x20) & 0x00FF;
// 				ReadMultipleBuffer [3] = 0x1F;
// 			}
// 			else if (Tag_Density == ISO15693_HIGH_DENSITY)
// 			{
// 				ReadMultipleBuffer [0] = 0x0A;
// 				ReadMultipleBuffer [2] = (NumSectorToRead*0x20) & 0x00FF;
// 				ReadMultipleBuffer [3] = ((NumSectorToRead*0x20) & 0xFF00) >>8;
// 				ReadMultipleBuffer [4] = 0x1F;
// 			}
//			memset (RepBuffer, 0x00,140);

// 			if (Tag_Density == ISO15693_LOW_DENSITY)
// 					CR95HF_SendRecv (0x04,ReadMultipleBuffer,RepBuffer);

// 			else if (Tag_Density == ISO15693_HIGH_DENSITY)
// 					CR95HF_SendRecv (0x05,ReadMultipleBuffer,RepBuffer);
			if ( ISO15693_ReadMultipleBlock (Requestflags, 0x00,NumSectorToRead,0x1F,RepBuffer ) !=RESULTOK)
						return ERRORCODE_GENERIC;	
			/*Data Temp*/
			memcpy(&Data_To_Read[NthDataToRead*128],&RepBuffer[3],128);
	}
				
	return RESULTOK;
}



/**
* @brief  Read Single Block in the TAG
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  *Data_To_Read : return the data read in the TAG
* @param  NbBlock_To_Read : Number of block to read in the TAG
* @param  FirstBlock_To_Read : First block to read in the TAG
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	
uint8_t ISO15693_Read_Single_Tag_Data(uint8_t Tag_Density, uint8_t *Data_To_Read, uint16_t NbBlock_To_Read, uint16_t FirstBlock_To_Read)
{
uint8_t /*ReadBuffer [4]={0x02, 0x20, 0x00, 0x00},*/
				RepBuffer[16],
				Requestflags = 0x02;
uint16_t NthDataToRead= 0x0000;
uint16_t Num_DataToRead = FirstBlock_To_Read;

		// update the RequestFlags
		if (Tag_Density == ISO15693_LOW_DENSITY)
			Num_DataToRead &=  0x00FF;
		else if (Tag_Density == ISO15693_HIGH_DENSITY)
			Requestflags = 0x0A;	
		else
			return ERRORCODE_GENERIC;

	for ( NthDataToRead= 0; NthDataToRead < NbBlock_To_Read; NthDataToRead++)
	{			
					Num_DataToRead += NthDataToRead;
								
// 					if (Tag_Density == ISO15693_LOW_DENSITY)
// 					{
// 						Num_DataToRead &=  0x00FF;
// 						ReadBuffer [2] = (Num_DataToRead) & 0x00FF;
//  					memset (RepBuffer, 0x00,8);
//  					CR95HF_SendRecv (0x03,ReadBuffer,RepBuffer);
// 					}
// 					else if (Tag_Density == ISO15693_HIGH_DENSITY)
// 					{
// 					ReadBuffer [0] = 0x0A;	
// 					ReadBuffer [2] = (Num_DataToRead) & 0x00FF;
// 					ReadBuffer [3] = ((Num_DataToRead) & 0xFF00) >> 8;
// 					memset (RepBuffer, 0x00,8);
// 					CR95HF_SendRecv (0x04,ReadBuffer,RepBuffer);						
// 					}
// 					else
// 						return ERRORCODE_GENERIC;

				if ( ISO15693_ReadSingleBlock (Requestflags, 0x00,Num_DataToRead,RepBuffer ) !=RESULTOK)
						return ERRORCODE_GENERIC;
					
 				 /*Data Temp*/
				 memcpy(&Data_To_Read[NthDataToRead*4],&RepBuffer[3],ISO15693_NBBYTE_BLOCKLENGTH);
	}
				
	return RESULTOK;
}


/**
* @brief  Read data by Bytes in the TAG (with read multiple if it is possible)
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  IC_Ref_Tag : The IC_Ref is use to use read multiple or read single
* @param  *Data_To_Read : return the data read in the TAG
* @param  NbBytes_To_Read : Number of Bytes to read in the TAG
* @param  FirstBytes_To_Read : First Bytes to read in the TAG
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	
uint8_t ISO15693_Read_Bytes_Tag_Data(uint8_t Tag_Density, uint8_t IC_Ref_Tag, uint8_t *Data_To_Read, uint16_t NbBytes_To_Read, uint16_t FirstBytes_To_Read)
{
/*0x80 => Page size read multiple 0x20 & 1 block = 4 bytes*/
uint16_t NbBlock_To_Read = NbBytes_To_Read/(0x80);
uint16_t FirstBlock_To_Read = FirstBytes_To_Read/4;
	
	/*LRiS2K don't support read multiple*/
	if(IC_Ref_Tag  == ISO15693_LRiS2K)
	{	
		NbBlock_To_Read = NbBytes_To_Read/4;		
		ISO15693_Read_Single_Tag_Data(Tag_Density, Data_To_Read, NbBlock_To_Read, FirstBlock_To_Read);
	}
	
	else
		ISO15693_Read_Multiple_Tag_Data(Tag_Density, Data_To_Read, NbBlock_To_Read, FirstBlock_To_Read);		
	
	memcpy(Data_To_Read,&Data_To_Read[FirstBytes_To_Read], NbBytes_To_Read);
	
	return RESULTOK;
	
}



/**
* @brief  Write data by blocks in the TAG
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  *Data_To_Write : Data to write in the TAG
* @param  NbBlock_To_Write : Number of block to write in the TAG
* @param  FirstBlock_To_Write : First block to write in the TAG
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	
uint8_t ISO15693_Write_Tag_Data(uint8_t Tag_Density, uint8_t *Data_To_Write, uint16_t NbBlock_To_Write, uint16_t FirstBlock_To_Write)
{
uint8_t WriteBuffer [8]={0x02, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
				RepBuffer[32],
				Requestflags =0x02;
uint16_t NthDataToWrite =0,
				 IncBlock =0;

		// update the RequestFlags
		if (Tag_Density == ISO15693_LOW_DENSITY)
			FirstBlock_To_Write &=  0x00FF;
		else if (Tag_Density == ISO15693_HIGH_DENSITY)
			Requestflags = 0x0A;	
		else
			return ERRORCODE_GENERIC;

		for ( NthDataToWrite=FirstBlock_To_Write; NthDataToWrite<(FirstBlock_To_Write+NbBlock_To_Write); NthDataToWrite++)
		{
// 				/*Block To Write*/				
// 				if(Tag_Density == ISO15693_LOW_DENSITY)	
// 				
// 				WriteBuffer [2] = (NthDataToWrite) & 0x00FF;

// 					else if(Tag_Density == ISO15693_HIGH_DENSITY)
// 				 {
// 				 WriteBuffer [0] = 0x0A;	 
// 				 WriteBuffer [2] = NthDataToWrite & 0x00FF;
// 				 WriteBuffer [3] = (NthDataToWrite& 0xFF00) >> 8;
// 				 }

// 				/*Data To Write in the Block*/
// 				WriteBuffer [3+Tag_Density] = Data_To_Write[0+(IncBlock)];
// 				WriteBuffer [4+Tag_Density] = Data_To_Write[1+(IncBlock)];
// 				WriteBuffer [5+Tag_Density] = Data_To_Write[2+(IncBlock)];
// 				WriteBuffer [6+Tag_Density] = Data_To_Write[3+(IncBlock)];

// 				 memset (RepBuffer, 0x00,32);
// 				 if (Tag_Density == ISO15693_LOW_DENSITY)
// 				 CR95HF_SendRecv (0x07,WriteBuffer,RepBuffer);

// 				 else if (Tag_Density == ISO15693_HIGH_DENSITY)
// 				 CR95HF_SendRecv (0x08,WriteBuffer,RepBuffer);
				 	if ( ISO15693_WriteSingleBlock (Requestflags, 0x00,NthDataToWrite,Data_To_Write,RepBuffer ) !=RESULTOK)
						return ERRORCODE_GENERIC;
				 
				 IncBlock += 4;
		}
					
	return RESULTOK;				
}	
	

/**
* @brief  Write data by bytes in the TAG
* @param  Tag_Density : TAG is HIGH or LOW density
* @param  *Data_To_Write : Data to write in the TAG
* @param  NbBytes_To_Write : Number of Bytes to write in the TAG
* @param  FirstBytes_To_Write : First Bytes to write in the TAG
* @retval ERRORCODE_GENERIC / RESULTOK.
*/	 
uint8_t ISO15693_Write_Bytes_Tag_Data(uint8_t Tag_Density, uint8_t *Data_To_Write, uint16_t NbBytes_To_Write, uint16_t FirstBytes_To_Write)
{
/*1 block = 4 bytes*/
uint16_t NbBlock_To_Write;

/*Convert in Blocks the number of bytes to write*/
uint16_t FirstBlock_To_Write = FirstBytes_To_Write/4;

uint8_t Length_Low_Limit,
				Length_High_Limit,
				Data_To_Save[8];
	
	NbBlock_To_Write = (NbBytes_To_Write/4)+1;
	
	/*Save the data of the uncomplete block to write*/
	ISO15693_Tag_Save(Tag_Density, NbBytes_To_Write, FirstBytes_To_Write, Data_To_Save, &Length_Low_Limit, &Length_High_Limit);
	
	
	memcpy(&Data_To_Write[-Length_Low_Limit],&Data_To_Save[0], Length_Low_Limit);
	memcpy(&Data_To_Write[NbBytes_To_Write],&Data_To_Save[4], Length_High_Limit);
	
	ISO15693_Write_Tag_Data(Tag_Density, &Data_To_Write[-Length_Low_Limit], NbBlock_To_Write, FirstBlock_To_Write);

	return RESULTOK;	
}

#endif

/**
 * @}
 */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

