#include	"pfm.h"
/**
  ******************************************************************************
  * @file    appmisc.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 Application support 
  *
  */
/** @addtogroup PFM6_Application
* @{
*/
//___________________________________________________________________________
void	Wait(int t,void (*f)(void)) {
int		to=__time__+t;
//			_DEBUG_MSG("waiting %d ms",t);
			while(to > __time__) {
				if(f)
					f();
			}
//			_DEBUG_MSG("... continue");
}
/*******************************************************************************
* Function Name : ScopeDumpBinary
* Description   :
* Input         :
* Output        :
* Return        :
*******************************************************************************/
int		ScopeDumpBinary(_ADCDMA *buf, int count) {

static _ADCDMA	*p=NULL;
static _io			*io=NULL;
static int			m=0;

int		i,j;
union	{
			_ADCDMA adc;
			char 		c[sizeof(_ADCDMA)];
			}	_ADC={0,0};

			if(buf) {														// call from parse !!!
				p=buf;
				m=count;
				io=__stdin.io;
				return 0;
			} else if(io && p && m) {
					io=_stdio(io);
					printf("-! %d,%d\r\n",m,4*pfm->ADCRate);
					io=_stdio(io);
					for(i=0;i<m;++i) {
						_ADC.adc.U += p[i].U;
						_ADC.adc.I += p[i].I;
						if((i%4) == 3) {
							_ADC.adc.U /=4;
							_ADC.adc.I /=4;
							io=_stdio(io);
							for(j=0; j<sizeof(_ADCDMA); ++j)
								printf("%c",_ADC.c[j]);
								io=_stdio(io);
								_ADC.adc.U =0;
								_ADC.adc.I =0;
							}
					}
				}
			return -1;
}
/*******************************************************************************
* Function Name : str2hex
* Description   : pretvorba stringa v hex stevilo
* Input         : **p, pointer na string array (char *), n stevilo znakov
* Output        : char * se poveca za stevilo znakov
* Return        : hex integer
*******************************************************************************/
int		str2hex(char **p,int n) {
char	q[16];
			strncpy(q,*p,n)[n]='\0';
			*p += n;
			return(strtoul(q,NULL,16));
			}
/*******************************************************************************/
/**
  * @brief  temperature interpolation from 	ADC data
  * @param t: ADC readout
  * @retval : temperature (10x deg.C)
  */
const int Ttab[]={											// tabela kontrolnih toèk
			2500,								  						// za interpolacijo tempreature
			5000,								  						// zaradi prec izracuna x 100
			8000,
			12500
			};

const	int Rtab[]={
			(4096.0*_Rdiv(47000.0,22000.0)),	// tabela vhodnih toèk za
			(4096.0*_Rdiv(18045.0,22000.0)),	// interpolacijo (readout iz ADC !!!)
			(4096.0*_Rdiv(6685.6,22000.0)),
			(4096.0*_Rdiv(1936.6,22000.0))
			};

_QSHAPE			qshape[_MAX_QSHAPE];
_USER_SHAPE	ushape[_MAX_USER_SHAPE];
			
#define	_K1											(_STATUS(p, PFM_STAT_SIMM1)/1)
#define	_K2											(_STATUS(p, PFM_STAT_SIMM2)/2)

#define _SHPMOD_OFF		0
#define _SHPMOD_MAIN	1
#define _SHPMOD_CAL		2
#define _SHPMOD_QSWCH	4

#define	_minmax(x,x1,x2,y1,y2) 	__min(__max(((y2-y1)*(x-x1))/(x2-x1)+y1,y1),y2)
/*******************************************************************************
* Function Name : SetPwmTab
* Description   : set the pwm sequence
* Input         : *p, PFM object pointer
* Return        :
*******************************************************************************/
void	SetPwmTab(PFM *p) {

_TIM18DMA
			*t=TIM18_buf;

int		i,j,n;
int		to			=p->Burst.Time;
int		tpause	=p->Burst.Length/p->Burst.N - p->Burst.Time;								// dodatek ups....
int		Uo=p->Burst.Pmax;
int		dUo=0;																															// modif. 2,3,4... pulza, v %
float	P2V = (float)_AD2HV(pfm->Burst.HVo)/_PWM_RATE_HI;
//-------wait for prev to finish ---
			while(_MODE(p,_PULSE_INPROC))
				Wait(2,App_Loop);
//-------user shape part -----------																			// 3. koren iz razmerja energij, ajde :)
			if(*(int *)ushape) {
				float e2E=pow(pow(P2V*p->Burst.Pmax,3)/400000.0*p->Burst.Time*p->Burst.N/(*(int *)ushape),1.0/3.0)/P2V;
				p->Burst.Length=0;
				for(i=1; ushape[i].T && i < _MAX_BURST/(10*_uS)-1 && i<_MAX_USER_SHAPE; ++t,++i) {
					t->n=2*ushape[i].T/10-1;
					t->T1=t->T2=_K1*(e2E*ushape[i].U + p->Burst.Pdelay);
					t->T3=t->T4=_K2*(e2E*ushape[i].U + p->Burst.Pdelay);
					p->Burst.Length+= ushape[i].T;
				}
										
				t->T1=t->T2=_K1*p->Burst.Psimm[0];
				t->T3=t->T4=_K2*p->Burst.Psimm[1];
				t->n=1;
				++t;
				t->T1=t->T2=_K1*p->Burst.Psimm[0];
				t->T3=t->T4=_K2*p->Burst.Psimm[1];
				t->n=0;
				
				p->Burst.Ereq=_SHPMOD_OFF;
				p->Burst.Einterval= __min(p->Burst.Length, _MAX_BURST/_MAX_ADC_RATE);

				_CLEAR_MODE(p, _P_LOOP);																					// current stab. off !!!
				return;
			}
//-------DELAY----------------------
			for(n=2*((p->Burst.Delay*_uS)/_PWM_RATE_HI)-1; n>0; n -= 255, ++t) {
				t->T1=t->T2=_K1*p->Burst.Pdelay;
				t->T3=t->T4=_K2*p->Burst.Pdelay;
				if(n > 255)
					t->n=255;
				else
					t->n=n;
			};
			
			p->Burst.Einterval= __min(p->Burst.Length + p->Burst.Delay, _MAX_BURST/_MAX_ADC_RATE);
//-------preludij-------------------
			if(p->Burst.Ereq & (_SHPMOD_CAL | _SHPMOD_QSWCH)) {
				int	du=0,u=0;
				for(i=0; i<_MAX_QSHAPE; ++i)
					if(p->Burst.Time==qshape[i].qref) {
						if(qshape[i].q0 > 0) {
							p->Burst.Einterval = __min(p->Burst.Einterval + qshape[i].q0, _MAX_BURST/_MAX_ADC_RATE);
							to=qshape[i].q0;
							Uo=(int)(pow((pow(p->Burst.Pmax,3)*p->Burst.N*qshape[i].qref/to),1.0/3.0)+0.5);
							if(p->Burst.Ereq & _SHPMOD_MAIN) {
								if(Uo > qshape[i].q1)
									Uo = qshape[i].q1;
							} else
								qshape[i].q1=Uo;
// prePULSE + delay
							for(n=((to*_uS)/_PWM_RATE_HI); n>0; n--,++t) 	{
								du+=(2*Uo-u-2*du)*70/qshape[i].q0;
								u+=du*70/qshape[i].q0;						
								t[0].T1= t[0].T2=_K1*(p->Burst.Pdelay + du + u*qshape[i].q2/100);
								t[0].T3= t[0].T4=_K2*(p->Burst.Pdelay + du + u*qshape[i].q2/100);
								t->n=1;
							}
//_______________________________________________________________________________________________________
// if Uo < q1 or calibrating prePULSE finish prePULSE & return
							if(Uo < qshape[i].q1 || p->Burst.Ereq == _SHPMOD_CAL) {
								while(du > p->Burst.Pdelay) 	{
									du+=(0-u-2*du)*70/qshape[i].q0; 
									u+=du*70/qshape[i].q0;
									t[0].T1= t[0].T2=_K1*(p->Burst.Pdelay + du + u*qshape[i].q2/100);
									t[0].T3= t[0].T4=_K2*(p->Burst.Pdelay + du + u*qshape[i].q2/100);
									t->n=1;
									++t;
									++to;
								}

								for(n=2*((p->Burst.Length*_uS/p->Burst.N-to*_uS)/_PWM_RATE_HI)-1;n>0;n -= 255,++t)	{
									t->T1=t->T2=_K1*p->Burst.Pdelay;
									t->T3=t->T4=_K2*p->Burst.Pdelay;
									if(n > 255)
										t->n=255;
									else
										t->n=n;
								}
//-------end of sequence------------						
								t->T1=t->T2=_K1*p->Burst.Psimm[0];
								t->T3=t->T4=_K2*p->Burst.Psimm[1];
								t->n=1;
								++t;
								t->T1=t->T2=_K1*p->Burst.Psimm[0];
								t->T3=t->T4=_K2*p->Burst.Psimm[1];
								t->n=0;
								return;
							}
						}
//_______________________________________________________________________________________________________
// else change parameters & continue to 1.pulse
//
						if(p->Burst.Ereq & _SHPMOD_QSWCH) {
							to=qshape[i].qref;
							Uo=p->Burst.Pmax;
							dUo=pow(p->Burst.Pmax*P2V,3) - 400000000 / qshape[i].qref * qshape[i].q3;				// varianta z zmanjsevanjem za fiksno E(J);
							if(dUo > 0)
								dUo=pow(dUo,1.0/3.0)/P2V - Uo;
							else
								dUo=-Uo;
//						dUo=(int)(pow(pow(Uo,3)*(float)qshape[i].q3/100.0,1.0/3.0))-Uo;									// varianta z zmanjsevanjem v % energije
//						dUo=(Uo * qshape[i].q3)/100 - Uo;																								// varianta z zmanjsevanjem v % napetosti
						} else {
							to=qshape[i].q3;
							Uo=(int)(pow((pow(p->Burst.Pmax,3)*p->Burst.N*qshape[i].qref - pow(qshape[i].q1,3)*qshape[i].q0)/qshape[i].qref/p->Burst.N,1.0/3.0)+0.5);
							tpause=_minmax(Uo,260,550,20,100);
						}
					}				
			}
			if(p->Burst.Ereq & _SHPMOD_MAIN) {
//-------PULSE----------------------
				for(j=0; j<p->Burst.N; ++j) {
					for(n=2*((to*_uS + _PWM_RATE_HI/2)/_PWM_RATE_HI)-1; n>0; n -= 255, ++t) {
						
						if(j == 0) {
							t->T1 = t->T2=_K1*(Uo+p->Burst.Pdelay);
							t->T3 = t->T4=_K2*(Uo+p->Burst.Pdelay);				
						} else {
							t->T1 = t->T2=_K1*(Uo+dUo+p->Burst.Pdelay);
							t->T3 = t->T4=_K2*(Uo+dUo+p->Burst.Pdelay);							
						}
						
						if(n > 255)
							t->n=255;
						else {
							t->n=n;
							p->Pockels.trigger=n;
						}
					}
//-------PAUSE----------------------			
					for(n=2*((tpause*_uS)/_PWM_RATE_HI)-1;n>0;n -= 255,++t)	{
						t->T1=t->T2=_K1*p->Burst.Pdelay;
						t->T3=t->T4=_K2*p->Burst.Pdelay;
						if(n > 255)
							t->n=255;
						else
							t->n=n;
					}
				}
			}
//------- fill seq. till end, except in user mode--------		
			if(p->Burst.Ereq != _SHPMOD_OFF) {
				for(n=2*((p->Burst.Length*_uS - p->Burst.N*(to+tpause)*_uS)/_PWM_RATE_HI)-1;n>0;n -= 255,++t)	{
					t->T1=t->T2=_K1*p->Burst.Pdelay;
					t->T3=t->T4=_K2*p->Burst.Pdelay;
					if(n > 255)
						t->n=255;
					else
						t->n=n;
				}
			}
//-------end of sequence------------						
			t->T1=t->T2=_K1*p->Burst.Psimm[0];
			t->T3=t->T4=_K2*p->Burst.Psimm[1];
			t->n=1;
			++t;
			t->T1=t->T2=_K1*p->Burst.Psimm[0];
			t->T3=t->T4=_K2*p->Burst.Psimm[1];
			t->n=0;
	}
/*______________________________________________________________________________
* Function Name : SetSimmerPw
* Description   : simmer pulse width
* Input         : None
* Output        : None
* Return        : None
*/
void		SetSimmerPw(PFM *p) {
_TIM18DMA	*t;

int 		psimm0=p->Burst.Psimm[0];																//		#kwwe723lwhd
int 		psimm1=p->Burst.Psimm[1];
	
			if(PFM_command(NULL,0) != _STATUS(p, PFM_STAT_SIMM1 | PFM_STAT_SIMM2)) {			
				if(PFM_command(NULL,0) & PFM_STAT_SIMM1)
						psimm1=psimm0;
				else
						psimm0=psimm1;
			}
			if(_MODE(p,_XLAP_SINGLE)) {
				if(_STATUS(p, PFM_STAT_SIMM1))
					TIM8->CCR2=TIM8->CCR1=TIM1->CCR2=TIM1->CCR1=psimm0;
				else
					TIM8->CCR2=TIM8->CCR1=TIM1->CCR2=TIM1->CCR1=0;
				if(_STATUS(p, PFM_STAT_SIMM2))
					TIM8->CCR4=TIM8->CCR3=TIM1->CCR4=TIM1->CCR3=psimm1;
				else
					TIM8->CCR4=TIM8->CCR3=TIM1->CCR4=TIM1->CCR3=0;
				for(t=TIM18_buf;t->n;++t) {
					t->T2=t->T1;
					t->T4=t->T3;
				};
			} else {
				if(_STATUS(p, PFM_STAT_SIMM1))  {
					TIM8->CCR1=TIM1->CCR1=psimm0;
					TIM8->CCR2=TIM1->CCR2=TIM1->ARR-psimm0;
				} else {
					TIM8->CCR1=TIM1->CCR1=0;
					TIM8->CCR2=TIM1->CCR2=TIM1->ARR;
				}
				if(_STATUS(p, PFM_STAT_SIMM2))  {
					TIM8->CCR3=TIM1->CCR3=psimm1;
					TIM8->CCR4=TIM1->CCR4=TIM1->ARR-psimm1;
				} else {
					TIM8->CCR3=TIM1->CCR3=0;
					TIM8->CCR4=TIM1->CCR4=TIM1->ARR;
				}
			for(t=TIM18_buf;t->n;++t) {
				t->T2=TIM1->ARR-t->T1;
				t->T4=TIM1->ARR-t->T3;
			}
	}
}
/*______________________________________________________________________________
*/
void	IncrementSimmerRate(int rate) {
static
int		r=0,flag;
			if(flag == (TIM1->CR1 & TIM_CR1_DIR))
				return;
			flag = (TIM1->CR1 & TIM_CR1_DIR);
			if(!r && !rate)
				return;
			if(rate)
				r=rate;
			else {
				if(TIM1->ARR < r) {
					++TIM1->ARR;
					++TIM8->ARR;
					if(!_MODE(pfm,_XLAP_SINGLE)) {
						++TIM1->CCR2;
						++TIM1->CCR4;
						++TIM8->CCR2;
						++TIM8->CCR4;
					}
				}
			}
}
/*______________________________________________________________________________
* Function Name : SetSimmerRate
* Description   : simmer pulse width
* Input         : None
* Output        : None
* Return        : None
*/
void	SetSimmerRate(PFM *p, SimmerType type) {										// #kd890304ri
int		simmrate;
	
			_CLEAR_MODE(pfm,_XLAP_SINGLE);
			_CLEAR_MODE(pfm,_XLAP_DOUBLE);
			_CLEAR_MODE(pfm,_XLAP_QUAD);
			
			if(type == _SIMMER_HIGH) {
				simmrate = _PWM_RATE_HI;
				_SET_MODE(pfm,_XLAP_QUAD);
			} else {
				if(PFM_command(NULL,0) &  PFM_STAT_SIMM1)
					simmrate=p->Burst.LowSimm[0];
				else
					simmrate=p->Burst.LowSimm[1];
				_SET_MODE(pfm,pfm->Burst.LowSimmerMode);
			}
			_DEBUG_MSG("simmer rate %3d kHz, mode %d", _mS/simmrate,pfm->mode % 0x07);

			while(!(TIM1->CR1 & TIM_CR1_DIR)) Watchdog();
			while((TIM1->CR1 & TIM_CR1_DIR)) Watchdog();
	
			TIM_CtrlPWMOutputs(TIM1, DISABLE);
			TIM_CtrlPWMOutputs(TIM8, DISABLE);

			TIM_SetCounter(TIM1,0);
			TIM_SetCounter(TIM8,0);

			TIM_Cmd(TIM1,DISABLE);
			TIM_Cmd(TIM8,DISABLE);			
			
			TIM_SetCounter(TIM1,0);
			TIM_SetCounter(TIM8,0);

			TIM_SetCounter(TIM1,simmrate/4);
			if(_MODE(p,_XLAP_QUAD))
				TIM_SetCounter(TIM8,3*simmrate/4);
			else
				TIM_SetCounter(TIM8,simmrate/4);

			TIM_SetAutoreload(TIM1,simmrate);
			TIM_SetAutoreload(TIM8,simmrate);
			SetSimmerPw(p);

			if(_MODE(p,_XLAP_SINGLE)) {
				TIM_OC2PolarityConfig(TIM1, TIM_OCPolarity_High);
				TIM_OC4PolarityConfig(TIM1, TIM_OCPolarity_High);
				TIM_OC2PolarityConfig(TIM8, TIM_OCPolarity_High);
				TIM_OC4PolarityConfig(TIM8, TIM_OCPolarity_High);
			} else {
				TIM_OC2PolarityConfig(TIM1, TIM_OCPolarity_Low);
				TIM_OC4PolarityConfig(TIM1, TIM_OCPolarity_Low);
				TIM_OC2PolarityConfig(TIM8, TIM_OCPolarity_Low);
				TIM_OC4PolarityConfig(TIM8, TIM_OCPolarity_Low);
			}

			if(_MODE(p,_PULSE_INPROC)) {
				TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
				TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
				TriggerADC(p);
			} else {
				TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
				TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);
			}

			TIM_CtrlPWMOutputs(TIM1, ENABLE);
			TIM_CtrlPWMOutputs(TIM8, ENABLE);
			TIM_Cmd(TIM1,ENABLE);
//_____________________________________________________________	
// 		if(!(p->Error  & _CRITICAL_ERR_MASK))
// 			EnableIgbt();
}
/*******************************************************************************/
void	EnableIgbt(void) {
			TIM_CtrlPWMOutputs(TIM1, ENABLE);
			TIM_CtrlPWMOutputs(TIM8, ENABLE);		
}
/*******************************************************************************/
void	DisableIgbt(void) {
			TIM_CtrlPWMOutputs(TIM1, DISABLE);
			TIM_CtrlPWMOutputs(TIM8, DISABLE);		
}
/*******************************************************************************/
int		fanPmin=20;
int		fanPmax=95;
int		fanTL=3000;
int		fanTH=4000;
/*******************************************************************************/
int		IgbtTemp(void) {
int		cc,t=__max( __fit(ADC3_buf[0].IgbtT1,Rtab,Ttab),
									__fit(ADC3_buf[0].IgbtT2,Rtab,Ttab));

			if(t<fanTL)
				cc=(_FAN_PWM_RATE*fanPmin)/200;
			else {
				if (t>fanTH)
					cc=(_FAN_PWM_RATE*fanPmax)/200;
				else
					cc=(_FAN_PWM_RATE*(((t-fanTL)*(fanPmax-fanPmin))/(fanTH-fanTL)+fanPmin	))/200;
			}
			cc=__min(_FAN_PWM_RATE/2-5,__max(5,cc));
			if(TIM_GetCapture1(TIM3) < cc)
				TIM_SetCompare1(TIM3,TIM_GetCapture1(TIM3)+1);
			else
				TIM_SetCompare1(TIM3,TIM_GetCapture1(TIM3)-1);
			return(t/100);
}
/*******************************************************************************/
/**
  * @brief	: fit 2 reda, Bronštajn str. 670
  * @param t:
  * @retval :
  */
int  	__fit(int to, const int t[], const int ft[]) {
int		f3=(ft[3]*(t[0]-to)-ft[0]*(t[3]-to)) / (t[0]-t[3]);
int		f2=(ft[2]*(t[0]-to)-ft[0]*(t[2]-to)) / (t[0]-t[2]);
int		f1=(ft[1]*(t[0]-to)-ft[0]*(t[1]-to)) / (t[0]-t[1]);
			f3=(f3*(t[1]-to) - f1*(t[3]-to)) / (t[1]-t[3]);
			f2=(f2*(t[1]-to)-f1*(t[2]-to)) / (t[1]-t[2]);
			return(f3*(t[2]-to)-f2*(t[3]-to)) / (t[2]-t[3]);
}
//	ADP1047 linear to float converter_____________________________________________
float	__lin2f(short i) {
		return((i&0x7ff)*pow(2,i>>11));
}
//	ADP1047 float to linear converter_____________________________________________
short	__f2lin(float u, short exp) {
		return ((((int)(u/pow(2,exp>>11)))&0x7ff)  | (exp & 0xf800));
}
/**
* @}
*/
/*
>1a
+e 0
*/
