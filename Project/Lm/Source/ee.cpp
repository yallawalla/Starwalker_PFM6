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
static		_EE*	me=NULL;
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void	_EE::ISR(_EE *p) {
			if(p) {																						
				me=p;																										// klic za prijavo instance
				me->buffer=_buffer_init(3*1000*sizeof(short));
			} else {																									// klic iz ISR, instanca in buffer morata bit ze formirana 																							
				switch(status) {
					case _IDLE:
						switch(phase)
						{
							case _tRD:
							case _tMRS:
							case _tRCV:
								break;
						}
						break;
					case _READ:
						if(nbits) {
							switch(phase) {
								case _tRD:
									EE_PORT->BSRRH   =  EE_BIT;										// _
									TIM2->ARR=_tRD-1;
									phase=_tMRS;
									break;
								case _tMRS:
									EE_PORT->BSRRL   =  EE_BIT;										// high
									TIM2->ARR=_tMRS-1;
									phase=_tRCV;
									break;
								case _tRCV:
									temp <<= 1;
									if(GPIO_ReadInputDataBit(EE_PORT,EE_BIT)==SET)
										temp |= 1;
									TIM2->ARR=_tRCV-1;
									if(--nbits == 1)
										_buffer_push(me->buffer,&temp,sizeof(char));
									if(nbits == 0) {
										phase=_tRD;
										status=_IDLE;
									}
									break;
							}
						}						
						break;
						
					case _WRITE:
						if(nbits) {
							switch(phase) {
								case _tRD:
									EE_PORT->BSRRH   =  EE_BIT;										//	low
									if(temp | 0x80)
										TIM2->ARR=_tRD-1;														//	_---
									else
										TIM2->ARR=_tRD + _tMRS-1;										//	___-
									phase=_tMRS;
									break;
								case _tMRS:
									if(temp | 0x80)
										TIM2->ARR=_tRCV + _tMRS-1;
									else
										TIM2->ARR=
									TIM2->ARR=_tRCV-1;
									phase=_tRCV;
									break;
								case _tRCV:
									TIM2->ARR=_tRCV-1;
									temp <<= 1;
									phase=_tRD;
									break;
							}
						}
						break;
						
					case _RESET:
						switch(phase)
						{
							case _tRD:
							case _tMRS:
							case _tRCV:
								break;
						}
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
