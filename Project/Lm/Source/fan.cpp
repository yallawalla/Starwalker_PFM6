/**
  ******************************************************************************
  * @file    dac.cpp
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 DA & DMA converters initialization
  *
  */ 
/** @addtogroup
* @{
*/

#include	"fan.h"
#include	"limits.h"
#include	"math.h"
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_FAN::_FAN() {
				fpl=20;
				fph=95;
				ftl=30;
				fth=40;
				tau=led=to=timeout=0;
				tacho = NULL;

#if defined (__DISCO__) || defined (__IOC_V1__)
				DAC_SetChannel2Data(DAC_Align_12b_R,0xfff);	
#elif defined (__IOC_V2__)
#else
	***error: HW platform not defined
#endif
				idx=0;
}
/*******************************************************************************
* Function Name	:
* Description		: 
* Output				:
* Return				: None
*******************************************************************************/
bool		_FAN::Poll() {
int			t=TIM_GetCapture2(TIM3);	
				if(t != to) {
					if(++led % 30 == 0)
						_YELLOW2(20);
					t-=to;
					if(t < 0)
						t += (1<<16);
					if(t>1000 && timeout < 30)
						tau=t;
					to=TIM_GetCapture2(TIM3);	
					timeout=0;
				}
				if(++timeout > 30)
					tau=EOF;

#if defined (__DISCO__) || defined (__IOC_V1__)
				DAC_SetChannel2Data(DAC_Align_12b_R,(int)(0xfff*(100-__minmax(Th2o(),ftl*100,fth*100,fpl,fph))/100));
#elif defined (__IOC_V2__)			
				TIM4->CCR1=(int)((TIM4->ARR*__minmax(Th2o(),ftl*100,fth*100,fpl,fph))/100);
#else
	***error: HW platform not defined
#endif				

				if(tacho && __time__ > 3000) {
					if(fabs(1.0 - tacho->Poly(Rpm())/(double)tau) > 0.2)
						printf("... fan tacho    %d,%d\r\n",(int)tacho->Poly(Rpm()),tau);
				}
				
				if(tau==INT_MAX)
					return true;
				else
					return false;
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
int			_FAN::Rpm(void) {
				return __minmax(Th2o(),ftl*100,fth*100,fpl,fph);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_FAN::LoadSettings(FILE *f) {
char		c[128];
				fgets(c,sizeof(c),f);
				sscanf(c,"%d,%d,%d,%d",&fpl,&fph,&ftl,&fth);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_FAN::SaveSettings(FILE *f) {
				fprintf(f,"%5d,%5d,%5d,%5d /.. fan\r\n",fpl,fph,ftl,fth);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_FAN::LoadLimits(FILE *f) {
char		c[128];
				tacho = new _FIT(4,FIT_POW);
	
				fgets(c,sizeof(c),f);
				sscanf(c,"%lf,%lf,%lf,%lf",&tacho->rp[0],&tacho->rp[1],&tacho->rp[2],&tacho->rp[3]);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_FAN::SaveLimits(FILE *f) {
				fprintf(f,"%lf,%lf,%lf,%lf\r\n",tacho->rp[0],		tacho->rp[1],		tacho->rp[2],		tacho->rp[3]);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
int			_FAN::Increment(int a, int b) {
				idx= __min(__max(idx+b,0),4);

				switch(idx) {
					case 1:
						fpl= __min(__max(fpl+a,5),fph);
						break;
					case 2:
						fph= __min(__max(fph+a,fpl),95);
						break;
					case 3:
						ftl= __min(__max(ftl+a,0),fth);
						break;
					case 4:
						fth= __min(__max(fth+a,ftl),50);
						break;
					}
				
				printf("\r:fan       %5d%c,%4.1lf'C",Rpm(),'%',(double)Th2o()/100);
				if(idx>0)
					printf("        %2d%c-%2d%c,%2d'C-%2d'C",fpl,'%',fph,'%',ftl,fth);		
				for(int i=4*(5-idx);idx && i--;printf("\b"));
				return Rpm();
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
bool		_FAN::Align(void) {
_FIT		*t;
	
int			_fpl=fpl,
				_fph=fph;

				if(tacho) {
					t=tacho; 
					tacho=NULL;
				} else {
					t = new _FIT(4,FIT_POW);
				}
				printf("\rfan  ");
				for(int i=10; i<90; i+=10) {
					fpl=fph=i;
					_wait(3000,_thread_loop);
					t->Sample(i,tau);
					printf(".");
				}
				for(int i=90; i>10; i-=10) {
					fpl=fph=i;
					_wait(3000,_thread_loop);
					t->Sample(i,tau);
					printf("\b \b");
				}
				
				fpl=_fpl;
				fph=_fph;
				
				if(!t->Compute())
					return false;
				else
					tacho=t;
					return true;
			}		
/**
* @}
*/ 

