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

#include	"pump.h"
#include	"limits.h"
#include	"isr.h"
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_PUMP::_PUMP() {
				tau=led=to=timeout=0;
				fpl=30;
				fph=95;
				ftl=25;
				fth=40;

				_ADC::Instance()->offset.cooler=12500;
				_ADC::Instance()->gain.cooler=13300;
	
#if defined (__DISCO__) || defined (__IOC_V1__)
				_DAC::Instance()->Dac1=0xfff;
#elif defined (__IOC_V2__)
				_DAC::Instance()->Dac1=0;
#else
	***error: HW platform not defined
#endif
				_DAC::Instance()->Refresh();
				idx=0;
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
bool		_PUMP::Poll(void) {
int			t=TIM_GetCapture1(TIM3);	
_DAC		*dac=_DAC::Instance();		
	
				if(t != to) {
					if(++led % 30 == 0)
						_BLUE2(20);
					if(timeout < 30) {
						if(t>to)
							tau=t-to;
						else
							tau=to-t;
					}
					to=t;
					timeout=0;
				}
				if(++timeout > 30)
					tau=INT_MAX;

#if defined (__DISCO__) || defined (__IOC_V1__)
				if(220000/__minmax(_ADC::Instance()->Th2o,ftl*100,fth*100,fpl,fph) < tau)
#elif defined (__IOC_V2__)
				if(220000/__minmax(_ADC::Instance()->Th2o,ftl*100,fth*100,fpl,fph) > tau)
#else
	***error: HW platform not defined
#endif
				dac->Dac1=__max(0x1ff,--dac->Dac1);
					else 
				dac->Dac1=__min(0xaff,++dac->Dac1);
				dac->Refresh();
	
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
int			_PUMP::Rpm(void) {
				return __minmax(_ADC::Instance()->Th2o,ftl*100,fth*100,fpl,fph);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_PUMP::LoadSettings(FILE *f) {
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
void		_PUMP::SaveSettings(FILE *f) {
				fprintf(f,"%5d,%5d,%5d,%5d /.. pump\r\n",fpl,fph,ftl,fth);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
int			_PUMP::Increment(int a, int b)	{
_ADC		*adc=_ADC::Instance();
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
				
				printf("\r:pump        %3d%c,%4.1lf'C,%4.1lf",Rpm(),'%',(double)adc->Th2o/100,(double)(adc->adf.cooler-adc->offset.cooler)/adc->gain.cooler);
				if(idx>0)
					printf("   %2d%c-%2d%c,%2d'C-%2d'C,%4.3lf",fpl,'%',fph,'%',ftl,fth,(double)adc->adf.Ipump/4096.0*3.3/2.1/16);		
				for(int i=4*(5-idx)+6;idx && i--;printf("\b"));
				return Rpm();
}
/**
* @}
*/ 


