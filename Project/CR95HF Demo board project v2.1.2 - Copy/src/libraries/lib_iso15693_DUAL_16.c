 //Inclure les fonctions qui seront utilisée
#include "stm32f10x.h"
#include "iso15693_DUAL_16.h"
#include "iso15693.h"
//#include "stm3210b_eval_lcd.h"
//#include "stm32_eval.h"

//extern uc8 ECHO[];

static uc8 EchoCmd[] = {0x55,0x00};

/*  protocol Select     
    Selects protocol and prepares for communication*/
bool ISO15693_SelectProtocole	(uint8_t TransmissionDataRate, uint8_t ReceptionDataRate)
{
	uint8_t DataRead[MAX_BUFFER_SIZE];
	if(!ISO15693_INITIALISATION(EchoCmd, DataRead))
	{
		return FALSE;
	}
	if(!ISO15693_INITIALISATION(SELECT_Protocol, DataRead))
	{
		return FALSE;
	}
	return TRUE;
}


/*Inventory	     
  When receiving the Inventory request, the  DUAL or LR runs the anticollision sequence.*/
bool ISO15693_Inventory( uint8_t *pDataRead )
{
	if(!ISO15693_INITIALISATION(INVENTORY, pDataRead))
	{
		return FALSE;
	}
	return TRUE;
}


   
    
/* When receiving the Get System Info command, the DUAL sends back its information
  data in the response
  Get Systeme info if addressed mode*/
bool ISO15693_GetSystemInfo_DUAL( uint8_t *pDataRead, uint8_t *ID )
{
   	uint8_t DataToSend[MAX_BUFFER_SIZE];
	memcpy(DataToSend    , GET_SYSTEM_INFO_DUAL, MAX_BUFFER_SIZE);
	memcpy(&DataToSend[4], ID    ,8);
	if(!ISO15693_INITIALISATION(DataToSend, pDataRead))
	{
		return FALSE;
	}
	return TRUE;
}


/*Get Systeme info if non addressed mode*/
bool ISO15693_GetSysInf_DUAL( uint8_t *pDataRead )
{
	if(!ISO15693_INITIALISATION(GET_SYS_INF_DUAL, pDataRead))
	{
		return FALSE;
	}
	return TRUE;
}

//Recherche DUAL16 A partir de UID

bool ISO15693_IsPresent( void )
{
	uint8_t DataRead[MAX_BUFFER_SIZE];
	/* Initializing buffer */
	memset(DataRead,0,MAX_BUFFER_SIZE);
	/* Init the ISO15693   communication */
	if(!ISO15693_SelectProtocole(0,0))
	{
		return FALSE;
	}
    /* Inventory */
	if(!ISO15693_Inventory(DataRead))
	{
		return FALSE;
	}
	/* An ISO15693 card is in the field */	
  return TRUE;
}



bool ISO15693_Anticollision(char *message)
{
	uint8_t i,DataRead[MAX_BUFFER_SIZE],ID[8],IC[8];
	
 	/*Inventory attempt*/ 
	if(!ISO15693_Inventory(DataRead))
	{
	    return FALSE;
	} 	
	ISO15693_DisplayINVENTORY(message, DataRead); 
	
	
	memcpy(IC+2,&DataRead[DATA_OFFSET+7],8);  

	/* Get System Info attemt */
	 for(i=0;i<1;i++)
	{
	 if((IC[DATA_OFFSET + i]==0x2C) || (IC[DATA_OFFSET + i]==0x2D) || (IC[DATA_OFFSET + i]==0x2E) || (IC[DATA_OFFSET + i]==0x2F) || (IC[DATA_OFFSET + i]==0x44) || (IC[DATA_OFFSET + i]==0x45) || (IC[DATA_OFFSET + i]==0x46) || (IC[DATA_OFFSET + i]==0x47))
	 {
   	 memcpy(ID,&DataRead[DATA_OFFSET+2],8);
	 if(!ISO15693_GetSysInf_ADD_DUAL(DataRead, ID))
	 {
	 return FALSE;
	 }
	 ISO15693_DisplayGET_SYS_INF_ADD_DUAL(message, DataRead); 
 	 
	 if(!ISO15693_GetSysInf_NOADD_DUAL(DataRead))
	 {
		return FALSE;
	 } 	
	 ISO15693_DisplayGET_SYS_INF_NOADD_DUAL(message, DataRead); 
	 }
	 }
	 return TRUE;
}

/*Get Systeme info if non addressed mode*/
// bibon function
bool ISO15693_GetSysInf_ADD_DUAL( uint8_t *pDataRead, uint8_t *ID  )

{
	if(!ISO15693_INITIALISATION(GET_SYS_INF_DUAL, pDataRead))
	{
		return FALSE;
	}
	return TRUE;
}			   


/*Get Systeme info if non addressed mode*/
// bibon function
bool ISO15693_GetSysInf_NOADD_DUAL			( uint8_t *pDataRead)
{
	if(!ISO15693_INITIALISATION(GET_SYS_INF_DUAL, pDataRead))
	{
		return FALSE;
	}
	return TRUE;
}


void ISO15693_DisplayINVENTORY	(char *message, uint8_t *pData)
{
	uint8_t i,DataRead[MAX_BUFFER_SIZE],ID[8],IC[8];

	sprintf(message, "%s 15693 detected !\n",message);
	sprintf(message, "%s Inventory Response =",message);
	for(i=0;i<pData[LENGTH_OFFSET];i++)
	{
		sprintf(message, "%s%02X",message, pData[DATA_OFFSET + i]);
	}

		
    sprintf(message, "%s\n ", message);
	sprintf(message, "%sID=",message);
	if(ISO15693_Inventory(DataRead))
	{
	memcpy(ID+2,&DataRead[DATA_OFFSET+2],8);
	for(i=0;i<8;i++)
	{
		//sprintf(message, "%s%02X",message, ID[DATA_OFFSET + i]);
	}
	}

	sprintf(message, "%s\n ", message);
	sprintf(message, "%sIC=",message);
	if(ISO15693_Inventory(DataRead))
	{
	memcpy(IC+2,&DataRead[DATA_OFFSET+7],8);
	for(i=0;i<1;i++)
	{
		//sprintf(message, "%s%02X  ",message, IC[DATA_OFFSET + i]);
		
	} 
    } 
	    
		sprintf(message, "%s\n ", message);
}		





void ISO15693_DisplayGET_SYS_INF_ADD_DUAL (char *message, uint8_t *pData)
{
    uint8_t i;
   	sprintf(message, "%s\n ", message);
	sprintf(message, "%sGet_System_Info  = ", message);

	for(i=0;i<pData[LENGTH_OFFSET];i++)
	{
		 sprintf(message, "%s%02X",message, pData[DATA_OFFSET + i]);
	}
	sprintf(message, "%s\n ", message);
}



void ISO15693_DisplayGET_SYS_INF_NOADD_DUAL (char *message, uint8_t *pData)
{
	uint8_t i, DataRead[MAX_BUFFER_SIZE],ID[8],IC[8],DSFID [16],AFI[16],MEMORY_SIZE[16],BLOC_SIZE[16];
    sprintf(message, "%sDUAL \n", message);
	sprintf(message, "%sGet_System_Info = \n ", message);

    for(i=0;i<pData[LENGTH_OFFSET];i++)
	{
		 sprintf(message, "%s%02X",message, pData[DATA_OFFSET + i]);
	}
 	
	sprintf(message, "%s\n ", message);	
	sprintf(message, "%sID=",message);
	if(ISO15693_Inventory(DataRead))
	{
	memcpy(ID+2,&DataRead[DATA_OFFSET+2],8);
	for(i=0;i<8;i++)
	{
		sprintf(message, "%s%02X",message, ID[DATA_OFFSET + i]);
	}
	}
	
	sprintf(message, "%s\n ", message);
	sprintf(message, "%sIC=",message);
	if(ISO15693_Inventory(DataRead))
	{
	memcpy(IC+2,&DataRead[DATA_OFFSET+7],8);
	for(i=0;i<1;i++)
	{
		sprintf(message, "%s%02X",message, IC[DATA_OFFSET + i]);
		
	} 
    }	 
	sprintf(message, "%s\n ", message);
	sprintf(message, "%sDSFID=",message);
	if(ISO15693_GetSysInf_NOADD_DUAL(DataRead))
	{
	memcpy(DSFID+2,&DataRead[DATA_OFFSET+10],16);
	for(i=0;i<1;i++)
	{
		sprintf(message, "%s%02X",message, DSFID[DATA_OFFSET + i]);
		
	} 
    }


	sprintf(message, "%s\n ", message);
	sprintf(message, "%sAFI=",message);
	if(ISO15693_GetSysInf_NOADD_DUAL(DataRead))
	{
	memcpy(AFI+2,&DataRead[DATA_OFFSET+11],16);
	for(i=0;i<1;i++)
	{
		sprintf(message, "%s%02X",message, AFI[DATA_OFFSET + i]);
		
	} 
    }
	
    sprintf(message, "%s\n ", message);
	sprintf(message, "%sMemory Size=",message);
	if(ISO15693_GetSysInf_NOADD_DUAL(DataRead))
	{
	memcpy(MEMORY_SIZE+2,&DataRead[DATA_OFFSET+12],16);
	for(i=0;i<2;i++)
	{
		sprintf(message, "%s%02X",message, MEMORY_SIZE[DATA_OFFSET + i]);
		
	} 
    }
	
	sprintf(message, "%s\n ", message);
	sprintf(message, "%sBlock Size=",message);
	if(ISO15693_GetSysInf_NOADD_DUAL(DataRead))
	{
	memcpy(BLOC_SIZE+2,&DataRead[DATA_OFFSET+14],16);
	for(i=0;i<1;i++)
	{
		sprintf(message, "%s%02X",message, BLOC_SIZE[DATA_OFFSET + i]);
		
	} 
    } 	

	sprintf(message, "%s\n ", message); 
}


