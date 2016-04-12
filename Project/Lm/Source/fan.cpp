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
					if(timeout < 30) {
						tau=t-to;
						if(tau < 0)
							tau += (1<<16);
					}
					to=t;
					timeout=0;
				}
				if(++timeout > 30)
					tau=INT_MAX;

#if defined (__DISCO__) || defined (__IOC_V1__)
				DAC_SetChannel2Data(DAC_Align_12b_R,(int)(0xfff*(100-__minmax(Th2o(),ftl*100,fth*100,fpl,fph))/100));
#elif defined (__IOC_V2__)			
				TIM4->CCR1=(int)((TIM4->ARR*__minmax(Th2o(),ftl*100,fth*100,fpl,fph))/100);
#else
	***error: HW platform not defined
#endif				
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
				
				printf("\r:fan         %3d%c,%4.1lf'C",Rpm(),'%',(double)Th2o()/100);
				if(idx>0)
					printf("        %2d%c-%2d%c,%2d'C-%2d'C",fpl,'%',fph,'%',ftl,fth);		
				for(int i=4*(5-idx);idx && i--;printf("\b"));
				return Rpm();
}
/**
* @}
*/ 

