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
#include	"pyro.h"
#include	"isr.h"
#include	"limits.h"
#include	"lpc.h"
static		_PYRO*	me;
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
#define _Ts				10																		// transmission bit time. us
#define _To				10																		// base sampling time, ms
#define _MAXBITS	32																		// number of bits

void	_PYRO::ISR(_PYRO *p) {
			if(p) {																						// klic za prijavo instance
				me=p;
				me->buffer=_buffer_init(3*1000*sizeof(short));
			} else																						// klic iz ISR, instanca in buffer morata bit ze formirana 																							
				if(nbits++) {
					temp = temp<<1;		
					if(GPIO_ReadInputDataBit(PYRO_PORT,PYRO_BIT)==SET)
						temp |= 1;
						TIM7->ARR=_Ts-1;
					} else
						TIM7->ARR=1000-(_MAXBITS-1)*_Ts-1;
					
					PYRO_PORT->BSRRH   =  PYRO_BIT;								// low
					PYRO_PORT->OTYPER	&= ~PYRO_BIT;								// PP
					PYRO_PORT->BSRRL   =  PYRO_BIT;								// high
					PYRO_PORT->OTYPER |=  PYRO_BIT;								// OD

					if(nbits > _MAXBITS) {
						count += _To;							
						if(enabled && count >= period) {
							count=0;
							short i=__time__ - sync;
							_buffer_push(buffer,&i,sizeof(short));
							i=temp &  0x3fff;
							_buffer_push(buffer,&i,sizeof(short));
							i=(short)((temp >> 14) - 0x1000);
							_buffer_push(buffer,&i,sizeof(short));
            }
						temp=nbits=0;
						TIM7->ARR=1000*(_To-1)-1;
					}
		}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_PYRO::_PYRO() {	
			nbits=temp=count=nsamples=0;
			period=10;
			sync=0;
			enabled=false;
			S1.numStages=0;
			ISR(this);
	
GPIO_InitTypeDef					
			GPIO_InitStructure;
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_Pin = PYRO_BIT;
			GPIO_Init(PYRO_PORT, &GPIO_InitStructure);
			GPIO_SetBits(PYRO_PORT,PYRO_BIT);

			TIM_TimeBaseInitTypeDef TIM;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
			TIM_TimeBaseStructInit(&TIM);
			TIM.TIM_Prescaler = (SystemCoreClock/2000000)-1;
			TIM.TIM_Period = 1000;
			TIM.TIM_ClockDivision = 0;
			TIM.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseInit(TIM7,&TIM);
			TIM_ARRPreloadConfig(TIM7,DISABLE);
			TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
			NVIC_EnableIRQ(TIM7_IRQn);
			TIM_Cmd(TIM7,ENABLE);

			memset (&S2,0, sizeof(arm_fir_instance_f32));
			arm_fir_init_f32(&S2, _MAX_TAPS, firCoeffs32, firStateF32, _BLOCKSIZE);
			memset(firStateF32,0,sizeof(firStateF32));
			memset(in,0,sizeof(in));
			memset(out,0,sizeof(out));
			
			for(int i=0; i< _MAX_TAPS; ++i)
				firCoeffs32[i]=1.0/_MAX_TAPS;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_PYRO::~_PYRO() {	
			TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE);
			NVIC_DisableIRQ(TIM7_IRQn);
			_buffer_close(buffer);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void	_PYRO::LoadSettings(FILE *f) {
char	c[128];
			fgets(c,sizeof(c),f);
			sscanf(c,"%d",&period);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void	_PYRO::SaveSettings(FILE *f) {
			fprintf(f,"%5d                   /.. pyro\r\n",period);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
int		_PYRO::Increment(int a, int b) {	
			period 		= __min(__max(10,period+10*a),2000);	
			printf("\r:thermopile  %3d",period);		
			return period;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	_PYRO::initFilter() {
			memset (&S1,0, sizeof(arm_biquad_casd_df1_inst_f32));
			nsamples=0;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	_PYRO::printFilter() {
			for(int i=0; i<S1.numStages; ++i)
				printf("\r\n+f %f,%f,%f,%f,%f",iirCoeffs32[5*i],iirCoeffs32[5*i+1],iirCoeffs32[5*i+2],iirCoeffs32[5*i+3],iirCoeffs32[5*i+4]);
}
/*******************************************************************************
* Function Name	:
* Description		:
* Output				:
* Return				:
*******************************************************************************/
void	_PYRO::addFilter(char *c) {
			int	i=S1.numStages;
			arm_biquad_cascade_df1_init_f32(&S1,i+1,iirCoeffs32,iirStateF32);
			sscanf(c,"%f,%f,%f,%f,%f",&iirCoeffs32[5*i],&iirCoeffs32[5*i+1],&iirCoeffs32[5*i+2],&iirCoeffs32[5*i+3],&iirCoeffs32[5*i+4]);
			memset(iirStateF32,0,++i*sizeof(float32_t));
			memset(in,0,_BLOCKSIZE*sizeof(float32_t));
			memset(out,0,_BLOCKSIZE*sizeof(float32_t));
			nsamples=0;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
int		_PYRO::addSample(int i) {
			in[nsamples]=i;			
			i=out[nsamples++];					
			if(nsamples==_BLOCKSIZE) {
				nsamples=0;
				if(S1.numStages > 0)
					arm_biquad_cascade_df1_f32(&S1, in, out, _BLOCKSIZE);
				else
					memcpy(out,in,sizeof(out));
//				int m,n;
//				for(m=n=0; n<_BLOCKSIZE; ++n)
//					m += out[n];
//				m /=_BLOCKSIZE;
//				for(int n=0; n<_BLOCKSIZE; ++n)
//					out[n]=m;
			}
			return i;
}

extern "C" {
/*******************************************************************************/
/**
	* @brief	TIM7_IRQHandler, klice staticni ISR handler, indikacija je NULL pointer,
	*					sicer pointer vsebuje parent class !!! Mora bit extern C zaradi overridanja 
						vektorjev v startupu
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void	TIM7_IRQHandler(void) {
			if (TIM_GetITStatus(TIM7,TIM_IT_Update) != RESET) {
				TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
				me->ISR(NULL);
				}
			}
}
/**
* @}
*/ 
