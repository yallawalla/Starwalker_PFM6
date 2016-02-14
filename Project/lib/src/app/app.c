/**
  ******************************************************************************
  * @file    app.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 Main PFM6 application functionality
  *
  */ 
/** @addtogroup PFM6_Application
* @{
*/
#include	"app.h"
#include	<string.h>
#include 	<stdarg.h>
#include 	<stdio.h>

FATFS			fsCpu,fsUsb;
volatile 
int				__time__,__debug__,__mode__,__status__,__error__,__event__;
/*______________________________________________________________________________
* Function Name : _thread_init
* Description   : Initialize PFM object
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void 			_thread_init(void) {
	
					RCC_AHB1PeriphClockCmd(ALL_GPIO,ENABLE);

					SysTick_init();
					SystemCoreClockUpdate();
					Initialize_NVIC();

					Watchdog_init(__IWDGHIGH__);
					_thread_add(Watchdog,NULL,"IWDG",0);

					__com0=Initialize_USART(921600);
					_thread_add(ParseCom,__com0,"ParseCom COM0",0);

					f_mount(&fsCpu,FSDRIVE_CPU,1);
					f_mount(&fsUsb,FSDRIVE_USB,1);

					_stdio(__com0);
					if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)
						printf("\r ... SWR reset, %dMHz\r\n>",SystemCoreClock/1000000);
					else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
						printf("\r ... IWDG reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)
						printf("\r ... WWDG reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)
						printf("\r ... power on reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)
					{} else {}
					RCC_ClearFlag();
					_stdio(NULL);
					ungets(__com0->rx,"@cfg.ini\r",9);
}
//______________________________________________________________________________________
_io				*ParseCom(_io *v) {
char 			*p;
int 			i,j;
_io				*io;

					if(v) {
						if(!v->parse)																	// first call init
							v->parse=DecodeCom;
						io=_stdio(v);
						do {
							i=fgetc(&__stdin);
							switch(i) {
								case _Eof:
									break;
								case _CtrlZ:
									while(1);
								case _CtrlY:
									NVIC_SystemReset();
								break;
								default:
									p=cgets(i,EOF);
									if(p) {
										while(*p==' ') ++p;
										j=v->parse(p);
										if(*p && j)
											printf("... WTF(%d)",j);						// error message
										v->parse(NULL);													// call newline
									}
							}
						} while(i != _Eof);
						_stdio(io);
					}
					return v;
}

/**
* @}
*/ 
