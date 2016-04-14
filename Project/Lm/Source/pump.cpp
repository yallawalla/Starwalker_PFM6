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
#include	"fit.h"
#include	"term.h"
#include	"isr.h"
#include	"math.h"
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

				tacho = pressure = current = NULL;
				offset.cooler=12500;
				gain.cooler=13300;
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
				if(t != to) {
					if(++led % 30 == 0)
						_BLUE2(20);
					t-=to;
					if(t < 0)
						t += (1<<16);
					if(t>1000 && timeout < 30)
						tau=t;
					to=TIM_GetCapture1(TIM3);	
					timeout=0;
				}
				if(++timeout > 30)
					tau=EOF;

				DAC_SetChannel1Data(DAC_Align_12b_R,__minmax(Th2o(),ftl*100,fth*100,fpl*0xfff/100,fph*0xfff/100));
				if(tacho && pressure && current && __time__ > 3000) {
					if(fabs(1.0 - tacho->Poly(Rpm())/(double)tau) > 0.1)
						printf("... pump tacho    %d,%d\r\n",(int)tacho->Poly(Rpm()),tau);
					if(fabs(1.0 - pressure->Poly(Rpm())/(double)adf.cooler) > 0.1)
						printf("... pump pressure %d,%d\r\n",(int)pressure->Poly(Rpm()),adf.cooler);
					if(fabs(1.0 - current->Poly(Rpm())/(double)adf.Ipump) > 0.1)
						printf("... pump current  %d,%d\r\n",(int)current->Poly(Rpm()),adf.Ipump);
				}
								
				if(tau==EOF)
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
				return __minmax(Th2o(),ftl*100,fth*100,fpl,fph);
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
void		_PUMP::LoadLimits(FILE *f) {
char		c[128];
				tacho = new _FIT(4,FIT_POW);
				pressure = new _FIT(4,FIT_POW);
				current = new _FIT(4,FIT_POW);
	
				fgets(c,sizeof(c),f);
				sscanf(c,"%lf,%lf,%lf,%lf",&tacho->rp[0],&tacho->rp[1],&tacho->rp[2],&tacho->rp[3]);
				fgets(c,sizeof(c),f);
				sscanf(c,"%lf,%lf,%lf,%lf",&pressure->rp[0],&pressure->rp[1],&pressure->rp[2],&pressure->rp[3]);
				fgets(c,sizeof(c),f);
				sscanf(c,"%lf,%lf,%lf,%lf",&current->rp[0],&current->rp[1],&current->rp[2],&current->rp[3]);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_PUMP::SaveLimits(FILE *f) {
				fprintf(f,"%lf,%lf,%lf,%lf\r\n",tacho->rp[0],		tacho->rp[1],		tacho->rp[2],		tacho->rp[3]);
				fprintf(f,"%lf,%lf,%lf,%lf\r\n",pressure->rp[0],pressure->rp[1],pressure->rp[2],pressure->rp[3]);
				fprintf(f,"%lf,%lf,%lf,%lf\r\n",current->rp[0],	current->rp[1],	current->rp[2],	current->rp[3]);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
int			_PUMP::Increment(int a, int b)	{
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

				printf("\r:pump      %5d%c,%4.1lf'C,%4.1lf",Rpm(),'%',(double)Th2o()/100,(double)(adf.cooler-offset.cooler)/gain.cooler);
				if(idx>0)
					printf("   %2d%c-%2d%c,%2d'C-%2d'C,%4.3lf",fpl,'%',fph,'%',ftl,fth,(double)adf.Ipump/4096.0*3.3/2.1/16);		
				for(int i=4*(5-idx)+6;idx && i--;printf("\b"));
				return Rpm();
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
bool		_PUMP::Align(void) {
_FIT		*t,*p,*c;
	
int			_fpl=fpl,
				_fph=fph;

				if(tacho && pressure && current) {
					t=tacho; 
					p=pressure; 
					c=current;
					tacho=pressure=current=NULL;
				} else {
					t = new _FIT(4,FIT_POW);
					p = new _FIT(4,FIT_POW);
					c = new _FIT(4,FIT_POW);	
				}
				printf("\rpump ");
				for(int i=20; i<50; ++i) {
					fpl=fph=i;
					_wait(200,_thread_loop);
					t->Sample(i,tau);
					p->Sample(i,(double)adf.cooler);
					c->Sample(i,(double)adf.Ipump);
					printf(".");
				}
				for(int i=50; i>20; --i) {
					fpl=fph=i;
					_wait(200,_thread_loop);
					t->Sample(i,tau);
					p->Sample(i,(double)adf.cooler);
					c->Sample(i,(double)adf.Ipump);
					printf("\b \b");
				}
				
				fpl=_fpl;
				fph=_fph;
				
				if(!t->Compute())
					return false;
				else if(!p->Compute())
					return false;
				else if(!c->Compute())
					return false;
				else
					tacho=t;
					pressure=p;
					current=c;
					return true;
			}		
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
bool		_PUMP::Test(void) {
int			_fpl=fpl,
				_fph=fph;

				if(!tacho || !pressure || !current)
					return false;

				do {
				for(int i=20; i<50; ++i) {
					fpl=fph=i;
					_wait(200,_thread_loop);
					printf("\r\n%4.3lf,%4.3lf,%4.3lf,%4.3lf,%4.3lf,%4.3lf",
																					tacho->Poly(Rpm()),(double)tau,
																					pressure->Poly(Rpm()),(double)(adf.cooler),
																					current->Poly(Rpm()),(double)(adf.Ipump));				
				}
				for(int i=50; i>20; --i) {
					fpl=fph=i;
					fpl=fph=i;
					_wait(200,_thread_loop);
					printf("\r\n%4.3lf,%4.3lf,%4.3lf,%4.3lf,%4.3lf,%4.3lf",
																					tacho->Poly(Rpm()),(double)tau,
																					pressure->Poly(Rpm()),(double)(adf.cooler),
																					current->Poly(Rpm()),(double)(adf.Ipump));				
				}
			} while(getchar() == EOF);
				printf("\r\n:");
				fpl=_fpl;
				fph=_fph;
				
				return true;
			}
/**
* @}
*/ 


