/**
	******************************************************************************
	* @file		pyro.cpp
	* @author	Fotona d.d.
	* @version
	* @date
	* @brief	thermopile sensor class
	*
	*/
	
/** @addtogroup
* @{
*/

#include	"ee.h"
#include	"isr.h"
#include	"limits.h"
static		_EE*	me;
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
#define _tBIT			25																		// transmission bit time. us
#define _tLOW			16																		// low timing
#define _tHIGH		2																			// number of bits
#define _tREAD		2																			// number of bits

void	_EE::ISR(_EE *p) {
			if(p) {																						// klic za prijavo instance
				me=p;
				me->buffer=_buffer_init(3*1000*sizeof(short));
			} else {																					// klic iz ISR, instanca in buffer morata bit ze formirana 																							
				switch(status) {
					case _eeIDLE:
						break;
//______________________________________________________________					
					case _eeWRITE:
						if(nbits < 16) {
							if(_buffer_pull(buffer,&temp,1)) {
								if(nbits++ % 2 == 0) {
									if(temp & 0x80)
										TIM2->ARR=_tLOW;
									else
										TIM2->ARR=_tHIGH;
									temp <<=1;
									EE_PORT->BSRRH  =  EE_BIT;						// low
								} else {
									TIM2->ARR = _tBIT - TIM2->ARR;
									EE_PORT->BSRRL  =  EE_BIT;						// high
								}
							}
						} else {
								if(nbits++ % 2 == 0)
									TIM2->ARR=_tHIGH;
								else {
									TIM2->ARR = _tBIT - TIM2->ARR;
									EE_PORT->BSRRL  =  EE_BIT;						// high
								}
							
						}
						break;
//______________________________________________________________
					case _eeRESET:
						TIM2->ARR=500;
						EE_PORT->BSRRH   =  EE_BIT;									// low
						status=_tRRT;
						break;
					case _tRRT:
						TIM2->ARR=10;
						EE_PORT->BSRRL   =  EE_BIT;									// high
						status=_eeIDLE;
						break;
//______________________________________________________________
					case tLOW0:
						TIM2->ARR=16;
						EE_PORT->BSRRH   =  EE_BIT;									// low
						status=tLOW1;
						break;
					case tLOW1:
						TIM2->ARR=25;
						EE_PORT->BSRRL   =  EE_BIT;									// high
						status=_eePROC;
						break;
//______________________________________________________________
					default:
						break;
				}
			}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_EE::_EE() {	
      io=_stdio(NULL);
      _stdio(io);
			nbits=temp=count=nsamples=0;
			enabled=false;
			ISR(this);
	
GPIO_InitTypeDef					
			GPIO_InitStructure;
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_Pin = EE_BIT;
			GPIO_Init(EE_PORT, &GPIO_InitStructure);
			GPIO_SetBits(EE_PORT,EE_BIT);

			TIM_TimeBaseInitTypeDef TIM;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
			TIM_TimeBaseStructInit(&TIM);
			TIM.TIM_Prescaler = (SystemCoreClock/2000000)-1;
			TIM.TIM_Period = 1000;
			TIM.TIM_ClockDivision = 0;
			TIM.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseInit(TIM2,&TIM);
			TIM_ARRPreloadConfig(TIM2,DISABLE);
			TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
			NVIC_EnableIRQ(TIM2_IRQn);
			TIM_Cmd(TIM2,ENABLE);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_EE::~_EE() {	
			TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
			NVIC_DisableIRQ(TIM2_IRQn);
			_buffer_close(buffer);
}
/**
* @}
*/ 
