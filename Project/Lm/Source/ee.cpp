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
#include	"stdio.h"
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

						
					case _WRITE:
						if(nbits) {
							switch(phase) {
								case _tRD:
									EE_PORT->BSRRH   =  EE_BIT;										//	low
									if(temp | (1<<nbits))
										TIM5->ARR=_tRD-1;														//	_---
									else
										TIM5->ARR=_tRD + _tMRS-1;										//	___-
									phase=_tMRS;
									break;
								case _tMRS:
									if(temp | (1<<nbits))
										TIM5->ARR=_tRCV + _tMRS-1;
									else
										TIM5->ARR=_tRCV;
									phase=_tRCV;
									break;
								case _tRCV:
									if(GPIO_ReadInputDataBit(EE_PORT,EE_BIT)==SET)
										temp |= 1<<nbits--;
									else
										temp &= ~(1<<nbits--);
									TIM5->ARR=_tRCV-1;
									phase=_tRD;
									break;
							}
						} else {
							status=_IDLE;
							TIM_Cmd(TIM5,DISABLE);
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
void	_EE::Exchg(char *c) {
int		i,j,t;
		while(sscanf(c,"%02X%01d",&i,&j)) {
			temp=i<<1;
			if(j=='-')
				temp |= 1;
			++c;++c;++c;
			status=_WRITE;
			phase=_tRD;
			nbits=9;
			t=__time__+5;
			TIM_Cmd(TIM5,ENABLE);
			while(status != _IDLE)
				if(__time__ > t)
					break;
			if(status == _IDLE) {
				if(temp % 2)
					printf(" %02X-",temp>>1);
				else
					printf(" %02X_",temp>>1);
			}	else {
				printf("... error");
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
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
			TIM_TimeBaseStructInit(&TIM);
			TIM.TIM_Prescaler = (SystemCoreClock/2000000)-1;
			TIM.TIM_Period = _tRD;
			TIM.TIM_ClockDivision = 0;
			TIM.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseInit(TIM5,&TIM);
			TIM_ARRPreloadConfig(TIM5,DISABLE);
			TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
			NVIC_EnableIRQ(TIM5_IRQn);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_EE::~_EE() {	
			TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE);
			NVIC_DisableIRQ(TIM5_IRQn);
			_buffer_close(buffer);
}
/**
* @}
*/ 
