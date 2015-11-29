
//Créer la fonction .h
#ifndef __ISO15693_DUAL_16_H
#define __ISO15693_DUAL_16_H

//Inclure les fonctions qui seront utilisée
//#include "strfnfca_lib.h"
//#include "error_code_strfnfca.h"
#include "string.h"
#include "stdio.h"
#include "drv_strfnfca.h"


//Définir les paramètres qui pouront être utilisée
#define RFU                 0
#define TRANSMISSION_26	    0
#define TRANSMISSION_52	    1
#define TRANSMISSION_6	    2
#define TRANSMISSION_RFU	3

#define WAIT_FOR_SOF	    1
#define MODULATION_100		0
#define SINGLE_SUBCARRIER   0
#define DUAL_SUBCARRIER     1

//#define MAX_BUFFER_SIZE  	255 

//Définir les commandes physique de la carte utilisé (DUAL 64K)


/* --------------------------------------------------
 * ISO 15693 commands -- CMD | Length | data
 * -------------------------------------------------- */

 
uc8 SELECT_Protocol[]     = {PROTOCOL_SELECT,                     // Cmd				
										    0x02,		          // Length
										    0x01,				  // protocol codes
										    0x09};				  // Parameter ( choice wait for SOF)


//uc8 INVENTORY[]			  = {SEND_RECEIVE,                       // Cmd				
//										  0x03,					 // Length
//										  0x26,					 // Request_flag  (One subcarrier ,Hight data rate , 1 slot)
//										  0x01,					 // inventory
//										  0x00};				 // mask length


//uc8 SELECT[] 		            = {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x25,					 // Select
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//
//
//uc8 STAY_QUIET[] 		        = {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x02,					 // Stay Quiet
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//
//
//
//uc8 RESET_TO_READY[] 		    = {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x26,					 // Reset to Ready
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//
//
//
//uc8 WRITE_AFI[] 		        = {SEND_RECEIVE,                         // Cmd				
//										       0x0B,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x27,					 // Write AFI
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00,					 // ID(7)
//											   0x00};					 // AFI
//
//
//uc8 WRITE_DSFID[] 		        = {SEND_RECEIVE,                         // Cmd				
//										       0x0B,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x29,					 // Write DSFID
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00,					 // ID(7)
//											   0x00};					 // DSFID



//uc8 LOCK_AFI[] 		            = {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x28,					 // lock_AFI
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//
//
//
//uc8 LOCK_DSFID[] 		        = {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//											   0x2A,					 // lock_DSFID
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//											   
//
//
//uc8 GET_SYSTEM_INFO_DUAL[]  	= {SEND_RECEIVE,                         // Cmd				
//										       0x0A,					 // Length			
//										       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//											   0x2B,					 // Get System Info
//										       0x00, 				     // ID(0)				
//										       0x00,					 // ID(1)			
//									       	   0x00,					 // ID(2)			
//										       0x00,					 // ID(3)			
//										       0x00,					 // ID(4)
//											   0x00,					 // ID(5)
//											   0x00,					 // ID(6)
//											   0x00};					 // ID(7)
//
//
//uc8 GET_SYS_INF_DUAL[] 	        = {SEND_RECEIVE,                         // Cmd				
//										       0x02,					 // Length			
//										       0x0A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 0, protocole extention flag is set to 1)
//											   0x2B};					 // Get System Inf0

											   

uc8 GET_SYSTEM_INFO_LR[] 		= {SEND_RECEIVE,                         // Cmd				
										       0x0A,					 // Length			
										       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
											   0x2B,					 // Get System Info
										       0x00, 				     // ID(0)				
										       0x00,					 // ID(1)			
									       	   0x00,					 // ID(2)			
										       0x00,					 // ID(3)			
										       0x00,					 // ID(4)
											   0x00,					 // ID(5)
											   0x00,					 // ID(6)
											   0x00};					 // ID(7)

uc8 GET_SYS_INF_LR[] 	        = {SEND_RECEIVE,                         // Cmd				
										       0x02,					 // Length			
										       0x02,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 0, protocole extention flag is set to 0)
											   0x2B};					 // Get System Inf0



//uc8 READ_SINGLE_BLOCK_DUAL[] 		= {SEND_RECEIVE,                         // Cmd				
//											       0x0C,					 // Length			
//											       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//												   0x20,					 // Read Single Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x20,					 //Block number
//												   0x00};					 //Block number
//
//uc8 READ_SINGLE_BLOCK_LR[] 		   = {SEND_RECEIVE,                         // Cmd				
//											       0x0B,					 // Length			
//											       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//												   0x20,					 // Read Single Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00};					 //Block number
//												   
//												   												   
//
//uc8 READ_MULTIPLE_BLOCK_LR[] 		= {SEND_RECEIVE,                         // Cmd				
//											       0x0C,					 // Length			
//											       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//												   0x23,					 // Read Multiple Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00,					 //	First Block Number
//												   0x05};					 //	Number of block
//
//uc8 READ_MULTIPLE_BLOCK_DUAL[] 		= {SEND_RECEIVE,                         // Cmd				
//											       0x0D,					 // Length			
//											       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//												   0x23,					 // Read Multiple Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00,					 //	First Block Number
//												   0x00,					 //	First Block Number
//												   0x05};					 //	Number of block

//uc8 WRITE_SINGLE_BLOCK_DUAL[] 		= {SEND_RECEIVE,                         // Cmd				
//											       0x10,					 // Length			
//											       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//												   0x21,					 // Write single Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00,					 //	Block number
//												   0x00,					 //	Block number
//												   0x00,					 //	Data
//												   0x00,					 //	Data
//												   0x00,					 //	Data
//												   0x00};					 //	Data
//
//
//uc8 WRITE_SINGLE_BLOCK_LR[] 		= {SEND_RECEIVE,                         // Cmd				
//											       0x0F,					 // Length			
//											       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//												   0x21,					 // Write single Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00,					 //	Block number
//												   0x00,					 //	Data
//												   0x00,					 //	Data
//												   0x00,					 //	Data
//												   0x00};					 //	Data
//
//
//uc8 LOCK_SINGLE_BLOCK_LR[] 		= {SEND_RECEIVE,                             // Cmd				
//											       0x0B,					 // Length			
//											       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//												   0x22,					 // lock single Block
//											       0x00, 				     // ID(0)				
//											       0x00,					 // ID(1)			
//										       	   0x00,					 // ID(2)			
//											       0x00,					 // ID(3)			
//											       0x00,					 // ID(4)
//												   0x00,					 // ID(5)
//												   0x00,					 // ID(6)
//												   0x00,					 // ID(7)
//												   0x00};					 //	Block number
//
//
//
//
//uc8 PRESENT_PASS_WORD_INITIAL_DUAL[] 		= {SEND_RECEIVE,                         // Cmd				
//													       0x10,					 // Length			
//													       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//														   0xB3,					 // present pass word  initial(00000000)
//														   0x02,					 // IC  Mfg Code
//													       0x00, 				     // ID(0)				
//													       0x00,					 // ID(1)			
//												       	   0x00,					 // ID(2)			
//													       0x00,					 // ID(3)			
//													       0x00,					 // ID(4)
//														   0x00,					 // ID(5)
//														   0x00,					 // ID(6)
//														   0x00,					 // ID(7)
//														   0x03,					 //	pass word number
//														   0x00,					 //	Data
//														   0x00,					 //	Data
//														   0x00,					 //	Data
//														   0x00};					 //	Data
//
//
//
//uc8 WRITE_SECTOR_PASS_WORD_DUAL[] 	    = {SEND_RECEIVE,                         // Cmd				
//												       0x10,					 // Length			
//												       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//													   0xB1,					 // Write sector pass word 
//													   0x02,					 // IC  Mfg Code
//												       0x00, 				     // ID(0)				
//												       0x00,					 // ID(1)			
//											       	   0x00,					 // ID(2)			
//												       0x00,					 // ID(3)			
//												       0x00,					 // ID(4)
//													   0x00,					 // ID(5)
//													   0x00,					 // ID(6)
//													   0x00,					 // ID(7)
//													   0x03,					 //	pass word number
//													   0x23,					 //	Data
//													   0x05,					 //	Data
//													   0x19,					 //	Data
//													   0x84};					 //	Data
//
//
//uc8 LOCK_SECTOR_PASS_WORD_DUAL[] 	    = {SEND_RECEIVE,                         // Cmd				
//												       0x0E,					 // Length			
//												       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//													   0xB2,					 // lock sector pass word 
//													   0x02,					 // IC Mfg Code
//												       0x00, 				     // ID(0)				
//												       0x00,					 // ID(1)			
//											       	   0x00,					 // ID(2)			
//												       0x00,					 // ID(3)			
//												       0x00,					 // ID(4)
//													   0x00,					 // ID(5)
//													   0x00,					 // ID(6)
//													   0x00,					 // ID(7)
//													   0x20,					 //	Sector number
//													   0x00,					 //	Sector number											   
//													   0x1D};					 //	Sector security status
//
//
//uc8 PRESENT_PASS_WORD_DUAL[] 				= {SEND_RECEIVE,                         // Cmd				
//													       0x10,					 // Length			
//													       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//														   0xB3,					 // present pass word  initial(00000000)
//														   0x02,					 // IC  Mfg Code
//													       0x00, 				     // ID(0)				
//													       0x00,					 // ID(1)			
//												       	   0x00,					 // ID(2)			
//													       0x00,					 // ID(3)			
//													       0x00,					 // ID(4)
//														   0x00,					 // ID(5)
//														   0x00,					 // ID(6)
//														   0x00,					 // ID(7)
//														   0x03,					 //	pass word number
//														   0x23,					 //	Data
//														   0x05,					 //	Data
//														   0x19,					 //	Data
//														   0x84};					 //	Data
//													   
//
//uc8 GET_MULTIPLE_BLOCK_STATUS_DUAL[] 		= {SEND_RECEIVE,                         // Cmd				
//													       0x0E,					 // Length			
//													       0x2A,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 1)
//														   0x2C,					 // Get Multiple Block Status
//													       0x00, 				     // ID(0)				
//													       0x00,					 // ID(1)			
//												       	   0x00,					 // ID(2)			
//													       0x00,					 // ID(3)			
//													       0x00,					 // ID(4)
//														   0x00,					 // ID(5)
//														   0x00,					 // ID(6)
//														   0x00,					 // ID(7)
//														   0x00,					 //	First block number
//														   0x00,					 //	First block number
//														   0x05,					 //	Number of Block
//														   0x00};					 //	Number of Block
//		

//uc8 GET_MULTIPLE_BLOCK_STATUS_LR[] 	    	= {SEND_RECEIVE,                         // Cmd				
//													       0x0C,					 // Length			
//													       0x22,					 //	Request_flag (One subcarrier ,Hight data rate ,Select_flag is set to 0, the Address_flag is set to 1, protocole extention flag is set to 0)
//														   0x2C,					 // Get Multiple Block Status
//													       0x00, 				     // ID(0)				
//													       0x00,					 // ID(1)			
//												       	   0x00,					 // ID(2)			
//													       0x00,					 // ID(3)			
//													       0x00,					 // ID(4)
//														   0x00,					 // ID(5)
//														   0x00,					 // ID(6)
//														   0x00,					 // ID(7)
//														   0x00,					 //	First block number														  														  
//														   0x05};					 //	Number of Block
//
//uc8 CARD_PRESENT2[]=  {SEND_RECEIVE,							   
// 										0x02,
//										0x02,
//										0x26}; 			   ////Reset to Ready
//



// Appelé les fonction définit dans (_.c)

bool ISO15693_SelectProtocole				( uint8_t TransmissionData, uint8_t ReceptionData );
bool ISO15693_Inventory			        	( uint8_t *pDataRead );
bool ISO15693_GetSysInf_ADD_DUAL			( uint8_t *pDataRead, uint8_t *ID  );
bool ISO15693_GetSysInf_NOADD_DUAL			( uint8_t *pDataRead);


/* ISO15693 interface --- */
bool ISO15693_IsPresent		   ( void );
bool ISO15693_Anticollision    ( char *message );
bool ISO15693_CardTest		   ( void );


void ISO15693_DisplayINVENTORY		        		( char *message, uint8_t *pData );
void ISO15693_DisplayGET_SYS_INF_ADD_DUAL   		( char *message, uint8_t *pData );
void ISO15693_DisplayGET_SYS_INF_NOADD_DUAL   		    ( char *message, uint8_t *pData );


#endif /* __ISO15693_H */

