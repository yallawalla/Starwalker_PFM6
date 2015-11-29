/**
  ******************************************************************************
  * @file    lib_M24LRXX.h 
  * @author  MMY Application Team
  * @version V0.1
  * @date    15/03/2011
  * @brief   
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
#ifndef __DUALM24LRXX_H
#define __DUALM24LRXX_H

#define M24LRXX_ICREF								0x2C
#define	M24LRXX_ERROR_CODE							0xFF

// M24LR64 specific command code
#define M24LRXX_ICCODE								0x02
#define M24LRXX_CMDCODE_WRITESECTORPWD				0xB1
#define M24LRXX_CMDCODE_LOCKSECTORPWD				0xB2
#define M24LRXX_CMDCODE_PRESENTSECTORPWD			0xB3
#define M24LRXX_CMDCODE_FASTREADSINGLEBLOCK			0xC0
#define M24LRXX_CMDCODE_FASTREADMULTIPLEBLOCK		0xC3
#define M24LRXX_CMDCODE_FASTINVENTORYINITIATED		0xC1
#define M24LRXX_CMDCODE_FASTINITIATE				0xC2
#define M24LRXX_CMDCODE_INVENTORYINITIATED			0xD1
#define M24LRXX_CMDCODE_INITIATE					0xD2

// data rates constants fot select commands
#define M24LRXX_TRANSMISSION_26					ISO15693_TRANSMISSION_26
#define M24LRXX_TRANSMISSION_53					ISO15693_TRANSMISSION_53
#define M24LRXX_TRANSMISSION_6	   	 			ISO15693_TRANSMISSION_6
#define M24LRXX_TRANSMISSION_RFU				ISO15693_TRANSMISSION_RFU
// constants fot select commands
#define M24LRXX_RESPECT_312	    				ISO15693_RESPECT_312
#define M24LRXX_WAIT_FOR_SOF	    			ISO15693_WAIT_FOR_SOF
// modulation constants fot select commands
#define M24LRXX_MODULATION_100					ISO15693_MODULATION_100
#define M24LRXX_MODULATION_10					ISO15693_MODULATION_10
// sub carrier constants fot select commands
#define M24LRXX_SINGLE_SUBCARRIER   			ISO15693_SINGLE_SUBCARRIER
#define M24LRXX_DUAL_SUBCARRIER     			ISO15693_DUAL_SUBCARRIER
// appendCrc constants fot select commands
#define M24LRXX_APPENDCRC  						ISO15693_APPENDCRC
#define M24LRXX_DONTAPPENDCRC     				ISO15693_DONTAPPENDCRC


// request flags 
#define M24LRXX_REQFLAG_SINGLESUBCARRIER  		ISO15693_REQFLAG_SINGLESUBCARRIER		
#define M24LRXX_REQFLAG_TWOSUBCARRIER     		ISO15693_REQFLAG_TWOSUBCARRIER	
#define M24LRXX_REQFLAG_LOWDATARATE  			ISO15693_REQFLAG_LOWDATARATE	
#define M24LRXX_REQFLAG_HIGHDATARATE     		ISO15693_REQFLAG_HIGHDATARATE	
#define M24LRXX_REQFLAG_INVENTORYFLAGNOTSET		ISO15693_REQFLAG_INVENTORYFLAGNOTSET	
#define M24LRXX_REQFLAG_INVENTORYFLAGSET   		ISO15693_REQFLAG_INVENTORYFLAGSET	
#define M24LRXX_REQFLAG_NOPROTOCOLEXTENSION		ISO15693_REQFLAG_NOPROTOCOLEXTENSION	
#define M24LRXX_REQFLAG_PROTOCOLEXTENSION  		ISO15693_REQFLAG_PROTOCOLEXTENSION	
	// request flag 5 to 8 definition when inventory flag is not set
#define M24LRXX_REQFLAG_NOTSELECTED				ISO15693_REQFLAG_NOTSELECTED	
#define M24LRXX_REQFLAG_SELECTED			   	ISO15693_REQFLAG_SELECTED	
#define M24LRXX_REQFLAG_NOTADDRESSES			ISO15693_REQFLAG_NOTADDRESSES	
#define M24LRXX_REQFLAG_ADDRESSED			   	ISO15693_REQFLAG_ADDRESSED	
#define M24LRXX_REQFLAG_OPTIONFLAGNOTSET		ISO15693_REQFLAG_OPTIONFLAGNOTSET	
#define M24LRXX_REQFLAG_OPTIONFLAGSET			ISO15693_REQFLAG_OPTIONFLAGSET  	
#define M24LRXX_REQFLAG_RFUNOTSET				ISO15693_REQFLAG_RFUNOTSET	
#define M24LRXX_REQFLAG_RFUSET					ISO15693_REQFLAG_RFUSET  	
	// request flag 5 to 8 definition when inventory flag is set
#define M24LRXX_REQFLAG_NOTAFI					ISO15693_REQFLAG_NOTAFI	
#define M24LRXX_REQFLAG_AFI				   		ISO15693_REQFLAG_AFI	
#define M24LRXX_REQFLAG_16SLOTS					ISO15693_REQFLAG_16SLOTS	
#define M24LRXX_REQFLAG_1SLOT					ISO15693_REQFLAG_1SLOT	   		
													
// password number ( M24LR64 embeded tree RF password (see memory map of M24LR64 datasheet))
#define M24LRXX_RFPASSWORD_NBBYTE					4
#define M24LRXX_RFPASSWORD_MIN						1
#define M24LRXX_RFPASSWORD_MAX						3					

#define M24LRXX_NBBYTE_UID	 						ISO15693_NBBYTE_UID

// sector number ( M24LR64 embeded tree RF password (see memory map of M24LR64 datasheet))
#define M24LRXX_SECTOR_NBBYTE						128		// 1kbits = 128 bytes
#define M24LRXX_SECTOR_MIN							0
#define M24LRXX_SECTOR_MAX							64
#define M24LRXX_SECTORNUMBER_NBBYTE					2		// sector number parameter 

// Block memory (see memory map of M24LR64 datasheet)
#define M24LRXX_BLOCK_NBBYTE						4		// 4 octets
#define M24LRXX_BLOCK_MIN							0
#define M24LRXX_BLOCK_MAX							2047
#define M24LRXX_MAXBLOCK_READMULTIPLEBLOCK			32
#define M24LRXX_MINBLOCK_READMULTIPLEBLOCK			1

// 
#define	M24LRXX_NBBYTE_MEMORYSIZE				  	3
#define	M24LRXX_NBBYTE_NBBLOCK					  	2


// secector secutity status 
#define M24LRXX_MASK_SECTORSECURITYSTATUS_LOCKBIT		0x01
#define M24LRXX_MASK_SECTORSECURITYSTATUS_USEDBIT		0x1F


// maximum size of tag command
#define M24LRXX_MAXLENGTH_WRITESINGLEBLOCK			17	 	// 8 + 8 + 64 + 16 + 32 + 16 = 136 bits => 17 bytes
#define M24LRXX_MAXLENGTH_WRITESECTORPWD			18	 	// 8 + 8 + 8 + 64 + 8 + 32 + 16 = 144 bits => 18 bytes
#define M24LRXX_MAXLENGTH_LOCKSECTOR				16	 	// 8 + 8 + 8 + 64 + 16 + 8 + 16 = 128 bits => 16 bytes
#define M24LRXX_MAXLENGTH_PRESENTSECTORPWD			18	 	// 8 + 8 + 8 + 64 + 8 + 32 + 16 = 144 bits => 18 bytes
#define M24LRXX_MAXLENGTH_READSINGLEBLOCK 			14		// 8 + 8 + 64 + 16 + 16 = 104bits => 14 bytes
#define M24LRXX_MAXLENGTH_READMULTIPLEBLOCK			16		// 8 + 8 + 8 + 64 + 16 + 8 + 16 = 128bits => 16 bytes
#define M24LRXX_MAXLENGTH_READMULBLOCK 				15		// 8 + 8 + 64 + 16 + 8 + 16 = 120 bits => 15 bytes
#define M24LRXX_MAXLENGTH_GETMULSECURITY			16		// 8 + 8 + 64 + 16 + 16 + 16 = 128 bits => 16 bytes
#define M24LRXX_MAXLENGTH_FASTREADSINGLEBLOCK		15		// 8 + 8 + 8 + 64 + 16 + 16 = 120 bits => 15 bytes
#define M24LRXX_MAXLENGTH_FASTINVENTORYINITIATED	14		// 8 + 8 + 8 + 8 + 64 + 16 = 112 bits => 14 bytes
#define M24LRXX_MAXLENGTH_INVENTORYINITIATED		14		// 8 + 8 + 8 + 8 + 64 + 16 = 112 bits => 14 bytes
#define M24LRXX_MAXLENGTH_FASTINITIATE				5		// 8 + 8 + 8 + 16 = 40 bits => 5 bytes



int8_t M24LRXX_SelectProtocol	(uc8 DataRate, uc8 TimeOrSOF, uc8 Modulation, uc8 SubCarrier, uc8 AppendCRC);

int8_t M24LRXX_Inventory						(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse);
int8_t M24LRXX_InventoryOneSlot					(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  );
int16_t M24LRXX_Inventory16Slots				(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *NbTag, uint8_t *pResponse  );
int8_t M24LRXX_SendEOF							(uint8_t *pResponse  );
int8_t M24LRXX_GetSystemInfo 					(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_ReadSingleBlock 					(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_WriteSingleBlock					(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 BlockLength,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_ReadMultipleBlocks 				(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_Select 							(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_ResetToReady 					(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_WriteAFI							(uc8 Flags, uc8 *UIDin,uc8 AFIToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_LockAFI 							(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_WriteDSFID						(uc8 Flags, uc8 *UIDin,uc8 DSFIDToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_LockDSFID 						(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_GetMultipleBlockSecutityStatus 	(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 *NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse);
int8_t M24LRXX_StayQuiet						(uc8 Flags,uc8 *UIDin,uc8 AppendCRC,uc8 *CRC16);

// M24LR64 specific commands
int8_t M24LRXX_WriteSectorPassword				(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_LockSectorPassword				(uc8 Flags, uc8 *UIDin, uc8 *SectorNumber,uc8 SectorSecurityStatus,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_PresentSectorPassword			(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_FastReadSingleBlock 				(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_FastReadMultipleBlock 			(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 NumberOfBlock,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_FastInventoryInitiated 			(uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_FastInitiate 					(uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_InventoryInitiated 				(uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t M24LRXX_Initiate 						(uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );


//int8_t M24LRXX_RunAntiCollision 				(uc8 Flags , uc8 AFI,uint8_t *NbTag,uint8_t *pUIDout);

int8_t M24LRXX_CreateRequestFlag (uc8 SubCarrierFlag,uc8 DataRateFlag,uc8 InventoryFlag,uc8 ProtExtFlag,uc8 SelectOrAFIFlag,uc8 AddrOrNbSlotFlag,uc8 OptionFlag,uc8 RFUFlag);


// Get functions
int8_t M24LRXX_GetUID 							(uint8_t *UIDout);
int8_t M24LRXX_GetDSFID 						(uint8_t *DSFIDout); 
int8_t M24LRXX_GetAFI 							(uint8_t *AFIout);
int8_t M24LRXX_GetMemSizeInfo 					(uint8_t *NbBlock,uint8_t *MemSize); 

int8_t M24LRXX_IsAM24LR64 						(uint8_t *Mask,uint8_t MaskLength);


// 	flags from information flags
int8_t M24LRXX_GetDSFIDFlag 				(uc8 FlagsByte);
int8_t M24LRXX_GetAFIFlag 					(uc8 FlagsByte);
int8_t M24LRXX_GetMemorySizeFlag 			(uc8 FlagsByte);
int8_t M24LRXX_GetICReferenceFlag 			(uc8 FlagsByte);
int8_t M24LRXX_GetIcRef 					(uint8_t *IcRefout);


int8_t M24LRXX_SplitInventoryResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t M24LRXX_SplitGetSystemInfoResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *InfoFlags,uint8_t *UIDout,uint8_t *DSFIDout,uint8_t *AFIout,uint8_t *MemSizeOut,uint8_t *IcRefOut,uint8_t *errorcode);
int8_t M24LRXX_SplitReadSingleBlockResponse		(uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);
int8_t M24LRXX_SplitWriteSingleBlockResponse	(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitWriteAFIResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitWriteDSFIDResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitLockAFIResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitLockDSFIDResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitReadMultipleBlockResponse	(uc8 *TagResponse,uc8 Length,uc8 NbBlock,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);
int8_t M24LRXX_SplitSelectResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitResetToReadyResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitGetMultipleBlockSecutityResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uint8_t *Flags ,uint8_t *DataOut,uint8_t *ErrorCode);
// M24LR64 specific response
int8_t M24LRXX_SplitWriteSectorPasswordResponse	(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitLockSectorPasswordResponse	(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitPresentSectorPasswordResponse(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t M24LRXX_SplitFastReadSingleBlockResponse	(uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);
int8_t M24LRXX_SplitFastInventoryInitiatedResponse(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t M24LRXX_SplitInventoryInitiatedResponse(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t M24LRXX_SplitFastInitiateResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t M24LRXX_SplitInitiateResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t M24LRXX_SplitFastReadMultipleBlockResponse	(uc8 *TagResponse,uc8 Length,uc8 NbBlock,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);

int8_t M24LRXX_SplitMemorySizeInfo				(uc8 *MemSizeInfo,uint8_t *BlockSize ,uint8_t *NbBlocks);


#endif
