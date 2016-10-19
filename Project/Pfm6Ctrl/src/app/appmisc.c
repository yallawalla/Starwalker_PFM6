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
/*******************************************************************************
* Function Name : Wait
* Description   :	recursive call to main loop !!!
* Input         :
* Output        :
* Return        :
*******************************************************************************/
void	Wait(int t,void (*f)(void)) {
int		to=__time__+t;
			while(to > __time__) {
				if(f)
					f();
			}
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
			unsigned char c[sizeof(_ADCDMA)];
			}	_ADC={0,0};

			if(buf) {														// call from parse !!!
				p=buf;
				m=count;
				io=__stdin.io;
				return 0;
			} else if(io && p && m) {
					io=_stdio(io);
					__print("-! %d,%d\r\n",m,4*pfm->ADCRate);
					io=_stdio(io);
					for(i=0;i<m;++i) {
						_ADC.adc.U += p[i].U;
						_ADC.adc.I += p[i].I;
						if((i%4) == 3) {
							_ADC.adc.U /=4;
							_ADC.adc.I /=4;
							io=_stdio(io);
							for(j=0; j<sizeof(_ADCDMA); ++j)
								while(__stdout.io->put(__stdout.io->tx,_ADC.c[j])==EOF)
									Watchdog();
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

#define _SHPMOD_OFF			0
#define _SHPMOD_MAIN		1
#define _SHPMOD_CAL			2
#define _SHPMOD_QSWCH		4

#define	_minmax(x,x1,x2,y1,y2) 	__min(__max(((y2-y1)*(x-x1))/(x2-x1)+y1,y1),y2)
/*******************************************************************************
* Function Name :  
* Description   : 
* Input         :  
* Return        :
*******************************************************************************/
_TIM18DMA *SetPwmTab00(PFM *p, _TIM18DMA *t) {
int		i,j,n;
int		to			=p->Burst.Time;
int		tpause	=p->Burst.Length/p->Burst.N - p->Burst.Time;								// dodatek ups....
int		Uo=p->Burst.Pmax;
int		dUo=0;																															// modif. 2,3,4... pulza, v %
float	P2V = (float)_AD2HV(p->HVref)/_PWM_RATE_HI;
//-------wait for prev to finish ---
			while(_MODE(p,_PULSE_INPROC))
				Wait(2,App_Loop);
//-------user shape part -----------																			// 3. koren iz razmerja energij, ajde :)
			if(*(int *)ushape) {
				float e2E=pow(pow(P2V*p->Burst.Pmax,3)/400000.0*p->Burst.Time*p->Burst.N/(*(int *)ushape),1.0/3.0)/P2V;
				p->Burst.Length=0;
				for(i=1; ushape[i].T && i < _MAX_BURST/(10*_uS)-1 && i<_MAX_USER_SHAPE; ++t,++i) {
					t->n=2*ushape[i].T/10-1;
					t->T=e2E*ushape[i].U + p->Burst.Pdelay;
				}
				p->Burst.Ereq=_SHPMOD_OFF;
//				p->Burst.Einterval= __min(p->Burst.Length, _MAX_BURST/_MAX_ADC_RATE);
				_CLEAR_MODE(p, _P_LOOP);																					// current stab. off !!!
				t->n=0;																														// EOF
				return t;																			
			}
//-------DELAY----------------------
			for(n=2*((p->Burst.Delay*_uS)/_PWM_RATE_HI)-1; n>0; n -= 256, ++t) {
				t->T=p->Burst.Pdelay;
				(n > 255) ? (t->n=255) : (t->n=n);
			};
//			p->Burst.Einterval= __min(p->Burst.Length + p->Burst.Delay, _MAX_BURST/_MAX_ADC_RATE);
//-------preludij-------------------
			if(p->Burst.Ereq & (_SHPMOD_CAL | _SHPMOD_QSWCH)) {
				int	du=0,u=0;
				for(i=0; i<_MAX_QSHAPE; ++i)
					if(p->Burst.Time==qshape[i].qref) {
						if(qshape[i].q0 > 0) {
//							p->Burst.Einterval = __min(p->Burst.Einterval + qshape[i].q0, _MAX_BURST/_MAX_ADC_RATE);
							to=qshape[i].q0;
							Uo=(int)(pow((pow(p->Burst.Pmax,3)*p->Burst.N*qshape[i].qref/to),1.0/3.0)+0.5);
							if(p->Burst.Ereq & _SHPMOD_MAIN) {
								if(Uo > qshape[i].q1)
									Uo = qshape[i].q1;
							} else
								qshape[i].q1=Uo;
//_______________________________________________________________________________________________________
// prePULSE + delay
							for(n=((to*_uS)/_PWM_RATE_HI); n>0; n--,++t) 	{
								du+=(2*Uo-u-2*du)*70/qshape[i].q0;
								u+=du*70/qshape[i].q0;						
								t->T=p->Burst.Pdelay + du + u*qshape[i].q2/100;
								t->n=1;
							}
//_______________________________________________________________________________________________________
// if Uo < q1 or calibrating prePULSE finish prePULSE & return
							if(Uo < qshape[i].q1 || p->Burst.Ereq == _SHPMOD_CAL) {
								while(du > p->Burst.Pdelay) 	{
									du+=(0-u-2*du)*70/qshape[i].q0; 
									u+=du*70/qshape[i].q0;
									t->T=p->Burst.Pdelay + du + u*qshape[i].q2/100;
									t->n=1;
									++t;
									++to;
								}

								for(n=2*((p->Burst.Length*_uS/p->Burst.N-to*_uS)/_PWM_RATE_HI)-1;n>0;n -= 256,++t)	{
									t->T=p->Burst.Pdelay;
									(n > 255) ? (t->n=255) : (t->n=n);
								}
								t->n=0;
								return t;
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
							tpause=_minmax(Uo,260,550,20,100);
							Uo=(int)(pow((pow(p->Burst.Pmax,3)*p->Burst.N*qshape[i].qref - pow(qshape[i].q1,3)*qshape[i].q0)/qshape[i].qref/p->Burst.N,1.0/3.0)+0.5);
						}
					}				
			}
			if(p->Burst.Ereq & _SHPMOD_MAIN) {
//-------PULSE----------------------
				for(j=0; j<p->Burst.N; ++j) {
//
// SWEEPS SMAFU .....................................................
					if(_MODE(p,__SWEEPS__) && p->Burst.Time == 50 && p->Burst.Length==1000 && p->Burst.N == 5 && p->Burst.Ereq == _SHPMOD_MAIN) {
						if(j==0) {
							tpause=10*abs((p->count % 60)-30)+250;
							Uo=(int)(pow(5.0/2.0*Uo*Uo*Uo,1.0/3.0)+0.5);
							dUo = Uo*(tpause - 550)/1000/3-Uo/50;
						}
						if(j==2)
							break;
					}
//..end  sweeps........................................
					
					for(n=2*((to*_uS + _PWM_RATE_HI/2)/_PWM_RATE_HI)-1; n>0; n -= 256, ++t) {
						
						if(j == 0) {
							t->T=Uo+p->Burst.Pdelay;			
						} else {
							t->T=Uo+dUo+p->Burst.Pdelay;					
						}
						
						(n > 255) ? (t->n=255) : (t->n=p->Pockels.trigger=n);
					}
//-------PAUSE----------------------			
					for(n=2*((tpause*_uS)/_PWM_RATE_HI)-1;n>0;n -= 256,++t)	{
						t->T=p->Burst.Pdelay;
						(n > 255) ? (t->n=255) : (t->n=n);
					}
				}
			}
//------- fill seq. till end, except in user mode--------		
			if(p->Burst.Ereq != _SHPMOD_OFF) {
				for(n=2*((p->Burst.Length*_uS - p->Burst.N*(to+tpause)*_uS)/_PWM_RATE_HI)-1;n>0;n -= 256,++t)	{
					t->T=p->Burst.Pdelay;
					(n > 255) ? (t->n=255) : (t->n=n);
				}
			}
			t->n=0;
			return t;
}
/*******************************************************************************
* Function Name : SetPwmTab
* Description   : Selects the pw buffer, according to burst & simmer parameters 
*								: Calls the waveform generator SetPwmTab00
*								: Calculates energy integratin interval to ADC
* Input         : *p, PFM object pointer
* Return        :
*******************************************************************************/
void	SetPwmTab(PFM *p) {
			int n;
			if(_STATUS(p,PFM_STAT_SIMM1) && !_STATUS(p,PFM_STAT_SIMM2)) {
				_TIM18DMA *t=SetPwmTab00(p,pwch1);
				for(n=0; t-- != pwch1; n+= t->n);
				p->Burst.Eint[0] = (n+1)*5;
			}
			else if(!_STATUS(p,PFM_STAT_SIMM1) && _STATUS(p,PFM_STAT_SIMM2)) {
				_TIM18DMA *t=SetPwmTab00(p,pwch2);
				for(n=0; t-- != pwch2; n+= t->n);
				p->Burst.Eint[1] = (n+1)*5;
			} else {
				_TIM18DMA *t = SetPwmTab00(p,pwch1);
				memcpy(pwch2,pwch1,sizeof(_TIM18DMA)*_MAX_BURST/_PWM_RATE_HI);
				for(n=0; t-- != pwch1; n+= t->n);
				p->Burst.Eint[0] = p->Burst.Eint[1] = (n+1)*5;
			}
}
/*______________________________________________________________________________
* Function Name : SetSimmerPw
* Description   : simmer pulse width
* Input         : None
* Output        : None
* Return        : None
*/
void	SetSimmerPw(PFM *p) {

int 	psimm0=p->Simmer[0].pw;																//		#kwwe723lwhd
int 	psimm1=p->Simmer[1].pw;
	
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
extern int _U1off,_U2off,_I1off,_I2off;
void	SetSimmerRate(PFM *p, SimmerType type) {										// #kd890304ri
int		simmrate;
	
			_CLEAR_MODE(pfm,_XLAP_SINGLE);
			_CLEAR_MODE(pfm,_XLAP_DOUBLE);
			_CLEAR_MODE(pfm,_XLAP_QUAD);
			
			if(type == _SIMMER_HIGH) {
				simmrate = _PWM_RATE_HI;
				_SET_MODE(pfm,pfm->Burst.Mode);
			} else {
				if(PFM_command(NULL,0) &  PFM_STAT_SIMM1) {
					simmrate=p->Simmer[0].rate;
					_SET_MODE(pfm,p->Simmer[0].mode);
				}	else {
					simmrate=p->Simmer[1].rate;
					_SET_MODE(pfm,p->Simmer[1].mode);
				}
			}
			
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

			if(_MODE(p,_PULSE_INPROC)) {
				_DEBUG_MSG("trigger at... %dV,%dA,%dV,%dA",_AD2HV(ADC3_AVG*ADC1_simmer.U),_AD2I(ADC1_simmer.I-_I1off),
																											_AD2HV(ADC3_AVG*ADC2_simmer.U),_AD2I(ADC2_simmer.I-_I2off));	
			} else {
				_DEBUG_MSG("simmer %3d kHz, mode %d", _mS/simmrate,pfm->mode & 0x07);
			}
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
/*******************************************************************************
* Function Name : __lin2f
* Description   :	ADP1047 linear to float converter
* Input         :
* Output        :
* Return        :
*******************************************************************************/
float	__lin2f(short i) {
		return((i&0x7ff)*pow(2,i>>11));
}
/*******************************************************************************
* Function Name : __f2lin
* Description   : ADP1047 float to linear converter
* Input         :
* Output        :
* Return        :
*******************************************************************************/
short	__f2lin(float u, short exp) {
		return ((((int)(u/pow(2,exp>>11)))&0x7ff)  | (exp & 0xf800));
}
/*******************************************************************************
* Function Name : SetChargerVoltage
* Description   :	ADP1047 output voltage setup, using the default format
* Input         :
* Output        :
* Return        :
*******************************************************************************/
int		SetChargerVoltage(int u) {
struct	
			{signed int e:3;} 
e3;
int		i=_VOUT_MODE;

			if(readI2C(__charger6,(char *)&i,1))	{
				e3.e=i;
				i = _VOUT+((u<<(8-e3.e))&~0xff);
					if(writeI2C(__charger6,(char *)&i,3))
						return _PARSE_OK;
			}
			return _PARSE_ERR_NORESP;
}
/*******************************************************************************
* Function Name : batch
* Description   :	ADP1047 output voltage setup, using the default format
* Input         :
* Output        :
* Return        :
*******************************************************************************/
int	batch(char *filename) {
FIL		f;
FATFS	fs;

			if(f_chdrive(0)==FR_OK && f_mount(0,&fs)==FR_OK && f_open(&f,filename,FA_READ)==FR_OK) {
				__print("\r\n>");
				__stdin.io->file=&f;
				do
					ParseCom(__stdin.io);
				while(!f_eof(&f));
				__stdin.io->file=NULL;
				f_close(&f);
				f_mount(0,NULL);
				return _PARSE_OK;
			} else
				return _PARSE_ERR_OPENFILE;
}
/*******************************************************************************
* Function Name : batch
* Description   :	ADP1047 output voltage setup, using the default format
* Input         :
* Output        :
* Return        :
*******************************************************************************/
void	CAN_console(void) {
char	c[128];
int		i,j;
			__dbug=__stdin.io;
			_SET_DBG(pfm,_DBG_CAN_COM);
			printf("\r\n remote console open... \r\n>");
			sprintf(c,">%02X%02X%02X",_ID_SYS2PFMcom,'v','\r');
			DecodeCom(c);
			do {
				for(i=0; i<8; ++i) {
					j=getchar();
					if(j == EOF || j == _CtrlE)
						break;
					sprintf(&c[2*i+3],"%02X",j);
				}
				if(i > 0)
					DecodeCom(c);
				App_Loop();
			} while (j != _CtrlE);
			sprintf(c,">%02X",_ID_SYS2PFMcom);
			DecodeCom(c);
			_CLEAR_DBG(pfm,_DBG_CAN_COM);
			__dbug=NULL;
			printf("\r\n ....remote console closed\r\n>");
}
/*******************************************************************************
* Function Name : batch
* Description   :	ADP1047 output voltage setup, using the default format
* Input         :
* Output        :
* Return        :
*******************************************************************************/
int			__print(char *format, ...) {

	char 		buf[128],*p;
	va_list	aptr;
	int			ret;
	
	va_start(aptr, format);
	ret = vsnprintf(buf, sizeof(buf), format, aptr);
	va_end(aptr);
	for(p=buf; *p; ++p)
		while(fputc(*p,&__stdout)==EOF)
			Wait(2,App_Loop);
  return(ret);
}
/**
* @}
*/
/*
>1a
+e 0
*/

