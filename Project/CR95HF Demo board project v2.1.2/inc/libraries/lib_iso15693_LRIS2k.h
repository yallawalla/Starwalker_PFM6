/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : iso15693.h
* Author             : MCD Application Team
* Version            : V3.2.0RC2
* Date               : 10/06/2010
* Description        : send iso15693 commands
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IS015693LRIS2K_H
#define __IS015693LRIS2K_H

#define LRIS2K_ICREF_VALUE							0x28
#define LRIS2K_ICREF_MASK							0xFC
#define	LRIS2K_ERROR_CODE							0xFF
				

// LRIS2k specific command code
#define LRIS2K_ICCODE								0x02
#define LRIS2K_CMDCODE_WRITEPASSWORD				0xB1
#define LRIS2K_CMDCODE_LOCKSECTORPWD				0xB2
#define LRIS2K_CMDCODE_PRESENTSECTORPWD				0xB3
#define LRIS2K_CMDCODE_FASTREADSINGLEBLOCK			0xC0
#define LRIS2K_CMDCODE_FASTREADMULTIPLEBLOCK		0xC3
#define LRIS2K_CMDCODE_FASTINVENTORYINITIATED		0xC1
#define LRIS2K_CMDCODE_FASTINITIATE					0xC2
#define LRIS2K_CMDCODE_INVENTORYINITIATED			0xD1
#define LRIS2K_CMDCODE_INITIATE						0xD2


// password number ( M24LR64 embeded tree RF password (see memory map of M24LR64 datasheet))
#define LRIS2K_RFPASSWORD_NBBYTE					4
#define LRIS2K_RFPASSWORD_MIN						1
#define LRIS2K_RFPASSWORD_MAX						3

// sector number ( M24LR64 embeded tree RF password (see memory map of M24LR64 datasheet))
#define LRIS2K_SECTOR_NBBYTE						128		// 1kbits = 128 bytes
#define LRIS2K_SECTOR_MIN							0
#define LRIS2K_SECTOR_MAX							64
#define LRIS2K_SECTORNUMBER_NBBYTE					2		// sector number parameter 

// Block memory (see memory map of M24LR64 datasheet)
#define LRIS2K_BLOCK_NBBYTE							4		// 4 octets
#define LRIS2K_BLOCK_MIN							0
#define LRIS2K_BLOCK_MAX							2047
#define LRIS2K_MAXBLOCK_READMULTIPLEBLOCK			32
#define LRIS2K_MINBLOCK_READMULTIPLEBLOCK			1

// 
#define	LRIS2K_NBBYTE_MEMORYSIZE				  	3
#define	LRIS2K_NBBYTE_NBBLOCK					  	2


// secector secutity status 
#define LRIS2K_MASK_SECTORSECURITYSTATUS_LOCKBIT		0x01
#define LRIS2K_MASK_SECTORSECURITYSTATUS_USEDBIT		0x1F


// maximum size of tag command
#define LRIS2K_MAXLENGTH_WRITESINGLEBLOCK			17	 	// 8 + 8 + 64 + 16 + 32 + 16 = 136 bits => 17 bytes
#define LRIS2K_MAXLENGTH_WRITESECTORPWD				18	 	// 8 + 8 + 8 + 64 + 8 + 32 + 16 = 144 bits => 18 bytes
#define LRIS2K_MAXLENGTH_LOCKSECTOR					16	 	// 8 + 8 + 8 + 64 + 16 + 8 + 16 = 128 bits => 16 bytes
#define LRIS2K_MAXLENGTH_PRESENTSECTORPWD			18	 	// 8 + 8 + 8 + 64 + 8 + 32 + 16 = 144 bits => 18 bytes
#define LRIS2K_MAXLENGTH_READSINGLEBLOCK 			14		// 8 + 8 + 64 + 16 + 16 = 104bits => 14 bytes
#define LRIS2K_MAXLENGTH_READMULTIPLEBLOCK			16		// 8 + 8 + 8 + 64 + 16 + 8 + 16 = 128bits => 16 bytes
#define LRIS2K_MAXLENGTH_READMULBLOCK 				15		// 8 + 8 + 64 + 16 + 8 + 16 = 120 bits => 15 bytes
#define LRIS2K_MAXLENGTH_GETMULSECURITY				16		// 8 + 8 + 64 + 16 + 16 + 16 = 128 bits => 16 bytes
#define LRIS2K_MAXLENGTH_FASTREADSINGLEBLOCK		15		// 8 + 8 + 8 + 64 + 16 + 16 = 120 bits => 15 bytes
#define LRIS2K_MAXLENGTH_FASTINVENTORYINITIATED		14		// 8 + 8 + 8 + 8 + 64 + 16 = 112 bits => 14 bytes
#define LRIS2K_MAXLENGTH_INVENTORYINITIATED			14		// 8 + 8 + 8 + 8 + 64 + 16 = 112 bits => 14 bytes
#define LRIS2K_MAXLENGTH_FASTINITIATE				5		// 8 + 8 + 8 + 16 = 40 bits => 5 bytes

// maximum size of tag response
#define LRIS2K_MAXREPLYLENGTH_READSINGLEBLOCK 		8
#define LRIS2K_MAXREPLYLENGTH_LOCKBLOCK	 			4
#define LRIS2K_MAXREPLYLENGTH_WRITEBLOCK 			4


int8_t LRIS2K_SelectProtocol				(uc8 DataRate, uc8 TimeOrSOF, uc8 Modulation, uc8 SubCarrier, uc8 AppendCRC);

int8_t LRIS2K_Inventory						(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse);
int8_t LRIS2K_InventoryOneSlot				(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *pResponse  );
int16_t LRIS2K_Inventory16Slots				(uc8 Flags , uc8 AFI, uc8 MaskLength, uc8 *MaskValue, uc8 AppendCRC, uc8 *CRC16, uint8_t *NbTag, uint8_t *pResponse  );
int8_t LRIS2K_SendEOF						(uint8_t *pResponse  );
int8_t LRIS2K_GetSystemInfo 				(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_ReadSingleBlock 				(uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_WriteSingleBlock				(uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 BlockLength,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_ReadMultipleBlocks 			(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_Select 						(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_ResetToReady 					(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_WriteAFI						(uc8 Flags, uc8 *UIDin,uc8 AFIToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_LockAFI 						(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_LockSingleBlock 				(uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_WritePassword					(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *DataToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_LockPassword 					(uc8 Flags, uc8 *UIDin, uc8 BlockNumber,uc8 ProtectStatus,uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_WriteDSFID					(uc8 Flags, uc8 *UIDin,uc8 DSFIDToWrite,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_LockDSFID 					(uc8 Flags, uc8 *UIDin, uc8 AppendCRC,uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_GetMultipleBlockSecutityStatus 	(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber, uc8 *NbBlocks,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse);
int8_t LRIS2K_StayQuiet						(uc8 Flags,uc8 *UIDin,uc8 AppendCRC,uc8 *CRC16);

// LRIS2K specific commands
int8_t LRIS2K_FastReadSingleBlock 			(uc8 Flags, uc8 *UIDin, uc8 *BlockNumber,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_FastInventoryInitiated 		(uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_FastInitiate 					(uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_InventoryInitiated 			(uc8 Flags, uc8 AFIin, uc8 MaskLength,uc8 *Mask,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_Initiate 						(uc8 Flags,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );
int8_t LRIS2K_PresentPassword				(uc8 Flags, uc8 *UIDin, uc8 PasswordNumber,uc8 *Password,uc8 AppendCRC, uc8 *CRC16,uint8_t *pResponse );


int8_t LRIS2K_RunAntiCollision 				(uc8 Flags , uc8 AFI,uint8_t *NbTag,uint8_t *pUIDout);

int8_t LRIS2K_CreateRequestFlag (uc8 SubCarrierFlag,uc8 DataRateFlag,uc8 InventoryFlag,uc8 ProtExtFlag,uc8 SelectOrAFIFlag,uc8 AddrOrNbSlotFlag,uc8 OptionFlag,uc8 RFUFlag);


// Get functions
int8_t LRIS2K_GetUID 						(uint8_t *UIDout);
int8_t LRIS2K_GetDSFID 						(uint8_t *DSFIDout); 
int8_t LRIS2K_GetAFI 						(uint8_t *AFIout);
int8_t LRIS2K_GetMemSizeInfo 				(uint8_t *NbBlock,uint8_t *MemSize); 

int8_t LRIS2K_IsALRIS2K 					(uint8_t *Mask,uint8_t MaskLength);


	// 	flags from information flags
int8_t LRIS2K_GetDSFIDFlag 				(uc8 FlagsByte);
int8_t LRIS2K_GetAFIFlag 				(uc8 FlagsByte);
int8_t LRIS2K_GetMemorySizeFlag 		(uc8 FlagsByte);
int8_t LRIS2K_GetICReferenceFlag 		(uc8 FlagsByte);
int8_t LRIS2K_GetIcRef 					(uint8_t *IcRefout);


int8_t LRIS2K_SplitInventoryResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t LRIS2K_SplitGetSystemInfoResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *InfoFlags,uint8_t *UIDout,uint8_t *DSFIDout,uint8_t *AFIout,uint8_t *MemSizeOut,uint8_t *IcRefOut,uint8_t *errorcode);
int8_t LRIS2K_SplitReadSingleBlockResponse		(uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);
int8_t LRIS2K_SplitWriteSingleBlockResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitWriteAFIResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitWriteDSFIDResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitLockAFIResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitLockDSFIDResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitLockPasswordResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitWritePasswordResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitPresentPasswordResponse		(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitSelectResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitResetToReadyResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags ,uint8_t *ErrorCode);
int8_t LRIS2K_SplitGetMultipleBlockSecutityResponse( uc8 *TagResponse,uc8 Length,uc8 NbBlock,uint8_t *Flags ,uint8_t *DataOut,uint8_t *ErrorCode);
// LRIS2K specific response

int8_t LRIS2K_SplitFastReadSingleBlockResponse	(uc8 *TagResponse,uc8 Length,uc8 NbByteOfData,uc8 OptionFlag,uint8_t *Flags ,uint8_t *SecurityStatus,uint8_t *Data,uint8_t *ErrorCode);
int8_t LRIS2K_SplitFastInventoryInitiatedResponse(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t LRIS2K_SplitInventoryInitiatedResponse	(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t LRIS2K_SplitFastInitiateResponse			(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);
int8_t LRIS2K_SplitInitiateResponse				(uc8 *TagResponse,uc8 Length,uint8_t *Flags , uint8_t *DSFIDextract, uint8_t *UIDout);


// testing function
int8_t LRIS2K_InventoryScanChain (uint8_t *UIDin,uint8_t AFIin, uint8_t *ScanChainResult);
int8_t LRIS2K_ReadWriteSingleBlockScanChain (uint8_t *UIDin,uint8_t AFIin, uint8_t *ScanChainResult);




#endif
