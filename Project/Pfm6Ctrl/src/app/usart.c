/**
  ******************************************************************************
  * @file    serial.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 Serial port initialization & ISR
  *
  */

/** @addtogroup PFM6_Setup
* @{
*/
#if defined  (STM32F2XX)
#include		"stm32f2xx.h"
#elif defined (STM32F10X_HD)
#include		"stm32f10x.h"
#elif	undefined (STM32F2XX || STM32F10X_HD)
*** undefined target !!!!
#endif
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "io.h"

_io	*__com0,*__com1,*__dbug;
/*******************************************************************************
* Function Name  : DMA_Configuration
* Description    : Configures the DMA.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define USART1_DR_Base  USART1_BASE+4
#define RxBufferSize		256
#define TxBufferSize		256
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
void DMA_Configuration(_io *io)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

/*DMA Channel4 USART1 TX*/
	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_Base;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(io->tx->_buf);
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream7, ENABLE);

/*DMA Channel4 USART1 RX*/
	DMA_DeInit(DMA2_Stream2);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_Base;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(io->rx->_buf);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = RxBufferSize;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA2_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream2, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
}
//______________________________________________________________________________________
volatile int32_t 
		ITM_RxBuffer=ITM_RXBUFFER_EMPTY; 
int	__getDMA(_buffer *rx) {
int	i=0;
#ifdef __DISCO__
	if(ITM_CheckChar()) {
		i=ITM_ReceiveChar();
		_buffer_push(rx,&i,1);
	}		
#else
	if(DMA2_Stream2->NDTR)
		rx->_push=&(rx->_buf[rx->size - DMA2_Stream2->NDTR]);
	else
		rx->_push=rx->_buf;
#endif
	if(_buffer_pull(rx,&i,1))
		return i;
	else
		return EOF;
}
//______________________________________________________________________________________
int	__putDMA(_buffer *tx, int	c) {
#ifdef __DISCO__
	return	ITM_SendChar(c);
#else
static
	int n=0;

	if(DMA2_Stream7->NDTR==0)																					// end of buffer reached
		n=0;																														// reset static index
	if(n == TxBufferSize-1)																						// buffer full ?
		return(EOF);	
	
	DMA2_Stream7->CR &= ~0x00000001;																	// stream 7 off
	while(DMA2_Stream7->CR & 0x00000001);															// wait for HW

	DMA2_Stream7->M0AR=(uint32_t)(&tx->_buf[n-DMA2_Stream7->NDTR]);		// set DMA pointer to new character
	DMA2_Stream7->NDTR++;																							// increase DMA counter
	tx->_buf[n++]=c;																									// enter new character
	
	DMA2->HIFCR = 0x0F400000;																					// clear all flags
	DMA2_Stream7->CR |= 0x00000001;																		// stream 7 on
	return(c);
#endif
}
//______________________________________________________________________________________
_io *Initialize_USART(int speed) {

USART_InitTypeDef 			USART_InitStructure;
USART_ClockInitTypeDef  USART_ClockInitStructure;
GPIO_InitTypeDef				GPIO_InitStructure;
_io 										*io;
#if  defined (__PFM6__)
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);		
 	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		
#elif  defined (__DISCO__)
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);		
 	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);	
#else
	#### error, no HW defined
#endif
	io=_io_init(RxBufferSize,TxBufferSize);	// initialize buffer
	io->get= __getDMA;	
	io->put= __putDMA;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART1, &USART_ClockInitStructure);
 
	USART_InitStructure.USART_BaudRate = speed;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 */
	DMA_Configuration(io);
	USART_Cmd(USART1, ENABLE);
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		
	return io;
}
//______________________________________________________________________________________
//
// zamenjava za gets, ne èaka, vraèa pointer na string brez \r(!!!) ali NULL	
// èe je mode ECHO (-1) na
//							<cr> izpiše <cr><lf>
//							<backspace> ali <del> izpiše <backspace><space><backspace>	
//
//______________________________________________________________________________________
char		*cgets(int c, int mode)
{
_buffer		*p=__stdin.io->gets;
			
			if(!p)
				p=__stdin.io->gets=_buffer_init(__stdin.io->rx->size);
			switch(c) {
				case EOF:		
					break;
				case '\r':
				case '\n':
					*p->_push = '\0';
					p->_push=p->_pull=p->_buf;
					return(p->_buf);
				case 0x08:
				case 0x7F:
					if(p->_push != p->_pull) {
						--p->_push;
						if(mode)
							__print("\b \b");
					}
					break;
				default:
					if(p->_push != &p->_buf[p->size-1])
						*p->_push++ = c;
					else  {
						*p->_push=c;
						if(mode)
							fputc('\b',&__stdout);
					}
					if(mode) {
						if(isprint(c))
							fputc(c,&__stdout);
						else
							__print("%c%02X%c",'<',c,'>');
					}
					break;
			}
			return(NULL);
}
//______________________________________________________________________________________
int			strscan(char *s,char *ss[],int c)
{
			int		i=0;
			while(1)
			{
				while(*s==' ') ++s;
				if(!*s)
					return(i);

				ss[i++]=s;
				while(*s && *s!=c)
				{
					if(*s==' ')
						*s='\0';
					s++;
				}
				if(!*s)
					return(i);
				*s++=0;
			}
}
//______________________________________________________________________________________
int			hex2asc(int i)
{
			if(i<10)
				return(i+'0');
			else 
				return(i-10+'A');
}
//_____________________________________________________________________________________
int			asc2hex(int i)
{
			if(isxdigit(i))
			{
				if(isdigit(i))
					return(i-'0');
				else
					return(toupper(i)-'A'+0x0a);
			}
			else
				return(0);
}
//______________________________________________________________________________________
int			getHEX(char *p, int n)
{
			int	i=0;
			while(n-- && isxdigit(*p))
				i=(i<<4) | asc2hex(*p++);
			return(i);
}
//______________________________________________________________________________________
void		putHEX(unsigned int i,int n)
{
			if(n>1)
				putHEX(i>>4,n-1);
			fputc(hex2asc(i & 0x0f),&__stdout);
}
//_____________________________________________________________________________________
char		*endstr(char *p)
{
			while(*p)
				++p;
			return(p);
}
//_____________________________________________________________________________________
#define		HEXREC3
//_____________________________________________________________________________________
int			sDump(char *p,int n)
{
			int	i,j;

#ifdef	HEXREC1
			i=(int)p + ((int)p >> 8);
			if(n<252)
				j=n;
			else
				j=252;
			n -= j;
			i += (j+3);
			__print("\r\nS1%02X%04X",j+3,(int)p);
#endif

#ifdef	HEXREC2
			i=(int)p + ((int)p >> 8) + ((int)p >> 16);
			if(n<250)
				j=n;
			else
				j=250;
			n -= j;
			i += (j+4);
			__print("\r\nS2%02X%06X",j+4,(int)p);
#endif

#ifdef	HEXREC3
			i=(int)p + ((int)p >> 8)+ ((int)p >>16)+ ((int)p >> 24);
			if(n<248)
				j=n;
			else
				j=248;
			n -= j;
			i += (j+5);
			__print("\r\nS3%02X%08X",j+5,(int)p);
#endif
//_____________________________________________________________________________________
			while(j--)
			{
				i += *p;
				putHEX(*p++,2);
			}
			putHEX(~i,2);
			if(n)
				sDump(p,n);
			return(-1);
}
//_____________________________________________________________________________________
int			iDump(int *p,int n)
{
			int		i,j,k;
			union 	{int i; char c[sizeof(int)];} u;

#ifdef	HEXREC1
			i=(int)p + ((int)p >> 8);
			if(n < (255-3)/sizeof(int))
				j=n;
			else
				j=(255-3)/sizeof(int);
			n -= j;
			i += (sizeof(int)*j+3);
			__print("\r\nS1%02X%04X",sizeof(int)*j+3,(int)p);
#endif

#ifdef	HEXREC2
			i=(int)p + ((int)p >> 8) + ((int)p >> 16);
			if(n < (255-5)/sizeof(int))
				j=n;
			else
				j=(255-5)/sizeof(int);
			n -= j;
			i += (sizeof(int)*j+4);
			__print("\r\nS2%02X%06X",sizeof(int)*j+4,(int)p);
#endif

#ifdef	HEXREC3
			i=(int)p + ((int)p >> 8)+ ((int)p >>16)+ ((int)p >> 24);
			if(n < (255-7)/sizeof(int))
				j=n;
			else
				j=(255-7)/sizeof(int);
			n -= j;
			i += (sizeof(int)*j+5);
			__print("\r\nS3%02X%08X",sizeof(int)*j+5,(int)p);
#endif
//_____________________________________________________________________________________
			while(j--)
			{
				u.i=*p++;
				for(k=0; k<sizeof(int); ++k)
				{
					i += u.c[k]; 
					putHEX(u.c[k] ,2);
				}
			}
			putHEX(~i,2);
			if(n)
				iDump(p,n);
			return(-1);
}
//_____________________________________________________________________________________
int			sLoad(char *p)
{
			int	 err,k,n;
			char *q,*a=NULL;
			k=*++p;												// skip 'S', k='1','2','3'
			++p;
			err=n=getHEX(p,2);
			q=p;
			++q;++q;
			while(--n) {
				err += getHEX(q,2);
				++q;++q;
			}
			if(((~err) & 0xff) != getHEX(q,2))
				__print("...checksum error !");
			else {
				n=getHEX(p,2);				++p;++p;
				switch(k) {
				case '1':
					a=(char *)getHEX(p,4);	++p;++p;++p;++p;
					n=n-3;
					break;
				case '2':
					a=(char *)getHEX(p,6);	++p;++p;++p;++p;++p;++p;
					n=n-4;
					break;
				case '3':
					a=(char *)getHEX(p,8);	++p;++p;++p;++p;++p;++p;++p;++p;
					n=n-5;
					break;
				}
				while(n--) {
					*(char *)a=getHEX(p,2);
					++a;++p;++p;
				}
			}
			return(-1);
}
/**
* @}
*/ 
