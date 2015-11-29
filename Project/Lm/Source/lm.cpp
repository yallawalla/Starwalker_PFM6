/**
	******************************************************************************
	* @file		lm.cpp
	* @author	Fotona d.d.
	* @version
	* @date		
	* @class	_LM		
	* @brief	lightmaster application class
	*
	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <ctype.h>

#include "ff.h"
#include "lm.h"
#include "can.h"

_LM::_LM() {
	
      io=_stdio(NULL);
      _stdio(io);
	
			_thread_add((void *)Poll,this,(char *)"lm",1);
			_thread_add((void *)Display,this,(char *)"plot",1);						
			FIL f;
			if(f_open(&f,"0:/lm.ini",FA_READ) == FR_OK) {
				pyro.LoadSettings((FILE *)&f);
				pump.LoadSettings((FILE *)&f);
				fan.LoadSettings((FILE *)&f);
				spray.LoadSettings((FILE *)&f);
				ec20.LoadSettings((FILE *)&f);
				while(!f_eof(&f))
				Parse((FILE *)&f);
				f_close(&f);	
			}	else				
				printf("\r\n setup file error...\r\n:");

			printf("\r\n[F1]  - thermopile");
			printf("\r\n[F4]  - spray on/off");
			printf("\r\n[F5]  - pump");
			printf("\r\n[F6]  - fan");
			printf("\r\n[F7]  - spray");
			printf("\r\n[F11] - save settings");	
			printf("\r\n[F12] - exit app.    ");	
			printf("\r\nCtrlE - EC20 console ");	
			printf("\r\n");	
			printf("\r\nCtrlY - reset");	

			_12Voff_ENABLE;
			debug=(__DEBUG__)0;
			
// not used in the application
//
#ifdef	USE_LCD
#ifdef	__SIMULATION__
			plot.Clear();
			plot.Add(&spray.pComp,1.0,0.02, LCD_COLOR_GREEN);
			plot.Add(&spray.pBott,1.0,0.02, LCD_COLOR_CYAN);
			plot.Add(&spray.pAir,1.0,0.002, LCD_COLOR_MAGENTA);


//		plot.Add(&_ADC::Instance()->buf.compressor,_BAR(1),_BAR(1)*0.02, LCD_COLOR_GREEN);
//		plot.Add(&_ADC::Instance()->buf.bottle,_BAR(1),_BAR(1)*0.02, LCD_COLOR_CYAN);
//		plot.Add(&_ADC::Instance()->buf.air,_BAR(1),_BAR(1)*0.002, LCD_COLOR_MAGENTA);

#endif
#endif
//
//
//
//
//
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_LM::~_LM() {																			// destructor
			_thread_remove((void *)Poll,this);
			_thread_remove((void *)Print,this);
			_thread_remove((void *)Display,this);
}
/*******************************************************************************
* Function Name	:
* Description		:
* Output				:

* Return				:
*******************************************************************************/
void	_LM::Poll(void *v) {
			_LM *self = static_cast<_LM *>(v);
			_io *temp=_stdio(self->io);
	
			self->can.Parse(self);
			self->spray.Poll();

			_ADC::Instance()->Status();
	
			if(_ADC::Instance()->error.V24 == false) {
				self->fan.Poll();
				self->pump.Poll();
				_TIM::Instance()->Poll();
			}

#ifdef __SIMULATION__
			self->spray.Simulator();
#ifdef USE_LCD
//			if(!(++self->zzz % 10) && self->plot.Refresh())
//				self->lcd.Grid();
#endif
#endif

			_stdio(temp);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	_LM::Print(void *v) {
			_LM *self = static_cast<_LM *>(v);	
			_io*	temp=_stdio(self->io);
			_ADCDMA *adf=&_ADC::Instance()->adf;

			printf("%d,%d,%d,%d\r\n",adf->cooler,adf->bottle,adf->compressor,adf->air);

			_stdio(temp);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:	
* Return				:
*******************************************************************************/
void	_LM::Select(__SELECTED__ i) {
			if(i != item)
				printf("\r\n");
			item = i;
			Refresh();
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:	
* Return				:
*******************************************************************************/
#include	"lm.h"
void	_LM::Increment(int i, int j) {
_ADCDMA	*offset	=&_ADC::Instance()->offset;
_ADCDMA	*gain		=&_ADC::Instance()->gain;
_ADCDMA	*adf		=&_ADC::Instance()->adf;
			switch(item) {
				case PUMP:
					pump.Increment(i,j);
					break;
				case FAN:
					fan.Increment(i,j);
					break;
				case SPRAY:
					spray.Increment(i,j);
					break;
				case EC20:
					ec20.Increment(i,j,this);
					break;
				
				case PYRO:
					if(i || j || pyro.enabled) {
						pyro.enabled=false;
						pyro.Increment(i,j);
					}	else {
						pyro.enabled=true;
						printf("\r\n");
						plot.Clear();
						plot.Add(&plotA,0,1, LCD_COLOR_GREEN);
						plot.Add(&plotB,2813,10, LCD_COLOR_CYAN);
						plot.Add(&plotC,0,1, LCD_COLOR_YELLOW);
					}
					break;
			
				case CTRL_A:
					offset->cooler+=10*i;
					gain->cooler+=10*j;
					printf("\r:cooler.....  %5d,%5d",offset->cooler,gain->cooler);
					break;
				case CTRL_B:
					offset->bottle+=10*i;
					gain->bottle+=10*j;
					printf("\r:bottle.....  %5d,%5d",offset->bottle,gain->bottle);
					break;
				case CTRL_C:
					offset->compressor+=10*i;
					gain->compressor+=10*j;
				printf("\r:compressor...  %5d,%5d",offset->compressor,gain->compressor);
					break;
				case CTRL_D:
					offset->air+=10*i;
					gain->air+=10*j;
					printf("\r:air........  %5d,%5d",offset->air,gain->air);
					break;
				
				case PLOT_OFFSET:
					plot.Offset(i,j);
					break;
				case PLOT_SCALE:
					plot.Scale(i,j);
					break;
			}
}

typedef	enum {PARSE_OK,PARSE_SYNTAX,PARSE_ILLEGAL,PARSE_MISSING,PARSE_MEM} ERR_MSG;

extern "C" {
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				: _thread_add((void *)poll_callback,this,(char *)"lm",10);
*******************************************************************************/
int		_LM::PlusDecode(char *c) {
			if(*c) {
				switch(*c) {
					case 'P':
						_thread_add((void *)_LM::Print,this,(char *)"lm",strtoul(++c,NULL,0));
						break;
					case 'L': {
						int *cc=(int *)strtoul(++c,&c,0);
						while(*c)
							*cc++=(int)strtoul(++c,&c,0);
						}
						break;
					case 'W': {
						short *cc=(short *)strtoul(++c,&c,0);
						while(*c)
							*cc++=(short)strtoul(++c,&c,0);
						}
						break;
					case 'B': {
						char *cc=(char *)strtoul(++c,&c,0);
						while(*c)
							*cc++=(char)strtoul(++c,&c,0);
						}
						break;
					case 'D':
						while(*c)
							debug = (__DEBUG__)(debug | (1<<strtoul(++c,&c,10)));
						break;
					case 'f':
						pyro.addFilter(++c);
						break;
					default:
						*c=0;
						return PARSE_SYNTAX;
				}
				*c=0;
				return PARSE_OK;
			} else {
				*c=0;
				return PARSE_MISSING;
			}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				: _thread_add((void *)poll_callback,this,(char *)"lm",10);
*******************************************************************************/
int		_LM::WhatDecode(char *c) {
			if(*c) {
				switch(*c) {
					case 'f':
						pyro.printFilter();
						break;
					case 'D':
						printf(" %0*X ",2*sizeof(debug)/sizeof(char),debug);
						break;
					default:
						*c=0;
						return PARSE_SYNTAX;
				}
				*c=0;
				return PARSE_OK;
			} else {
				*c=0;
				return PARSE_MISSING;
			}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				: _thread_add((void *)poll_callback,this,(char *)"lm",10);
*******************************************************************************/
int		_LM::MinusDecode(char *c) {
			if(*c) {
				switch(*c) {
					case 'P':
						_thread_remove((void *)_LM::Print,this);
						break;
					case 'f':
						pyro.initFilter();
						break;
					case 'D':
						while(*c)
							debug = (__DEBUG__)(debug & ~(1<<strtoul(++c,&c,10)));
						break;
					default:
						*c=0;
						return PARSE_SYNTAX;
				}
				*c=0;
				return PARSE_OK;
			} else {
				*c=0;
				return PARSE_MISSING;
			}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				: _thread_add((void *)poll_callback,this,(char *)"lm",10);
*******************************************************************************/
int		_LM::Decode(char *c) {
			if(*c)
				switch(*c) {
					case 'v':
						PrintVersion(SW_version);
						break;
					case '?':
						return WhatDecode(++c);
					case '+':
						return PlusDecode(++c);
					case '-':
						return MinusDecode(++c);
					case 'L':
						printf(" %08X",*(int *)strtoul(++c,NULL,0));
						break;
					case 'W':
						printf(" %04X",*(int *)strtoul(++c,NULL,0));
						break;
					case '.':
						can.Send(++c);
						break;
					case '_':
						can.Recv(++c);
						break;
					case '#':	
					{
#define _PI 3.14159265359
						int dacoff=strtoul(++c,&c,0);
						int dacgain=strtoul(++c,&c,0);
						for(int i=0; i<sizeof(DacBuff)/sizeof(short); ++i)
							DacBuff[i]=dacoff + (double)dacgain*sin(2.0*_PI*(double)i/(sizeof(DacBuff)/sizeof(short)));	
						break;
					}
					case '!': {
						FIL 									f;
						WAVE_FormatTypeDef		w;
						short									nbytes, sample;
						char									flag=0;
						
#ifdef	USE_LCD
int					to=0;
//int					pref=0,
//						peak=0;
#endif
						plot.Clear();
						plot.Add(&plotA,0,5, LCD_COLOR_GREEN);
						plot.Add(&plotB,0,10, LCD_COLOR_CYAN);
//					plot.Add(&plotC,0,1, LCD_COLOR_YELLOW);

						if(f_open(&f,"0:/3.wav",FA_READ) == FR_OK) {
							if(f_read (&f, &w, sizeof(w), (UINT *)&nbytes)==FR_OK) {
								while(!f_eof(&f)) {
									_wait(3,_thread_loop);
									if(!flag) {
										f_read (&f, &sample, sizeof(sample),(UINT *)&nbytes);
										plotA=sample-6767;
										plotB=pyro.addSample(plotA);

//										if(peak==0) {															// falling..
//											if(plotB < pref) {
//												if(plotB < pref-50) {
//													peak=pref;
//													plotC=0;
//													printf("%d,%d\r\n",to,peak);
//												}
//											}
//											else
//												pref=plotB;
//										} else {																	// rising...
//											if(plotB > pref) {
//												if(plotB > pref + 50)
//													if(peak > 5) {
//														peak=0;
//														plotC=50;
//													}
//											}
//											else {
//												pref=plotB;
//											}
//										}	

#ifdef	USE_LCD
										to=(f_tell(&f)-sizeof(w))*3/2;
										if(plot.Refresh()) {
char									str[16];
											lcd.Grid();
											sprintf(str,"%d",to/1000);
											LCD_SetFont(&Font8x12);
											sFONT *fnt = LCD_GetFont();
											LCD_SetTextColor(LCD_COLOR_GREY);
											LCD_DisplayStringLine(1, (uint8_t *)str);
										}
#endif
									}
									switch(VT100.Escape()) {
										case EOF:
											break;
										case ' ':
											flag ^= 1;
											break;
										case 'l':
											f_lseek(&f, f_tell(&f) - 320);
											break;
										case 'r':
											f_lseek(&f, f_tell(&f) + 320);
											break;
										case 0x1b:
											f_lseek (&f, f.fsize);
											break;
										case __F2:case __f2:
											Select(PLOT_OFFSET);
											break;
										case __F3:case __f3:
											Select(PLOT_SCALE);
											break;
										case __Up:
											Increment(1, 0);
											break;				
										case __Down:
											Increment(-1, 0);
											break;	
										case __Left:
											Increment(0, -1);
											break;				
										case __Right:
											Increment(0, 1);
											break;	
									}
								}
							f_close(&f);
							}
						} else
							printf("\r\n file not found...\r\n:");
}
						break;

					default:
						*c=0;
						return PARSE_SYNTAX;					
				}
			*c=0;
			return PARSE_OK;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
int		lm() {
_LM 	lm;
			printf("\r\n:");						
			do
				_thread_loop();
			while(lm.Parse()==true);
			return 0;
}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
bool	_LM::Parse() {
			_stdio(io);
			return Parse(VT100.Escape());
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
bool	_LM::Parse(FILE *f) {
			return Parse(fgetc(f));
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
bool	_LM::Parse(int i) {
_ADCDMA	*offset	=&_ADC::Instance()->offset;
_ADCDMA	*gain		=&_ADC::Instance()->gain;
_ADCDMA	*adf		=&_ADC::Instance()->adf;

			switch(i) {
				case EOF:
					break;

				case __F1:
				case __f1:
					Select(PYRO);
					break;

				case __F2:
				case __f2:
					break;
				case __F3:
				case __f3:
					break;
				case __F4:
				case __f4:
					if(spray.mode.On)
						spray.mode.On=false;
					else
						spray.mode.On=true;
					break;

				case __F5:
				case __f5:
					Select(PUMP);
					VT100.Repeat(1000);
					break;
				case __F6:
				case __f6:
					Select(FAN);
					VT100.Repeat(1000);
					break;			
				case __F7:
				case __f7:
					Select(SPRAY);
					VT100.Repeat(1000);
					break;
				case __F8:
				case __f8:
					Select(EC20);
					Decode((char *)".2100");
					break;
				case __F9:
				case __f9:
					break;				
				case __F10:
				case __f10:
					break;				
				case __F11:
				case __f11:
					FIL f;
					if(f_open(&f,"0:/lm.ini",FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
							pyro.SaveSettings((FILE *)&f);
							pump.SaveSettings((FILE *)&f);
							fan.SaveSettings((FILE *)&f);
							spray.SaveSettings((FILE *)&f);
							ec20.SaveSettings((FILE *)&f);
							f_sync(&f);
							f_close(&f);							
							printf("\r\n saved...\r\n:");
						}	else				
							printf("\r\n file error...\r\n:");
						break;	

				case __F12:
				case __f12:
					return false;
				
				case __Up:
					Increment(1, 0);
					break;				
				case __Down:
					Increment(-1, 0);
					break;	
				case __Left:
					Increment(0, -1);
					break;				
				case __Right:
					Increment(0, 1);
					break;	
		
				case __CtrlE:
					Decode((char *)".BA");
					printf("\r\n:");
					break;	

				case __CtrlV:
					if(spray.vibrate)
						spray.vibrate=false;
					else
						spray.vibrate=true;
					break;

				case __CtrlI:
					*offset = *adf;
					printf("\r\n:offset...  %3d,%3d,%3d,%3d\r\n:",offset->cooler,offset->bottle,offset->compressor,offset->air);
					break;
					
				case __FOOT_OFF:
					printf("footswitch disconnected \r\n:");
					spray.mode.On=false;
					Decode((char *)".2200");
					break;	
				case __FOOT_IDLE:
					spray.mode.On=false;
					Decode((char *)".2200");
					break;	
				case __FOOT_MID:
					spray.mode.On=true;
					Decode((char *)".2200");
					break;	
				case __FOOT_ON:		
					spray.mode.On=true;
					Decode((char *)".2201");
					break;	
									
				case __CtrlY:
					NVIC_SystemReset();
				case __CtrlZ:
					while(1);

				case __CtrlA:
					Select(CTRL_A);
					break;
				case __CtrlB:
					Select(CTRL_B);
					break;
				case __CtrlC:
					Select(CTRL_C);
					break;
				case __CtrlD:
					Select(CTRL_D);
					break;

				case 0x08:
				case 0x7f:
					if(VT100.clp != VT100.cl) {
						--VT100.clp;
					printf("\b \b");
					}
					break;
				case 0x0d:
					*VT100.clp=0;
					VT100.clp=VT100.cl;
					i=Decode(VT100.clp);
					if(i)
						printf(" ...wtf(%02X)\r\n:",i);
					else
						printf("\r\n:");						
					break;
				case 0x0a:
					break;
				default:
					if(i & 0xff00)
						printf("<%X>\r\n:",i);
					else {
						printf("%c",i);
						*VT100.clp++=i;
					}
			}
			return true;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	_LM::Display(void *v) {
_LM 	*self = static_cast<_LM *>(v);	
_io*	temp=_stdio(self->io);
			while(_buffer_len(self->pyro.buffer) > 3*sizeof(short)) {
				short 	ta,tp,t;
//______ buffer pull from ISR __________________________________________________					
				_buffer_pull(self->pyro.buffer,&t,sizeof(short));							
				_buffer_pull(self->pyro.buffer,&ta,sizeof(short));
				_buffer_pull(self->pyro.buffer,&tp,sizeof(short));
//______ filter ________________________________________________________________			
				self->plotA=ta;
				self->plotB=self->pyro.addSample(ta+tp);
//______ print at F1____________________________________________________________							
				if(self->pyro.enabled && self->item == PYRO) {
//					printf("%4d,%5d,%3.1lf,%hu,%u",ta,(int)tp+0x8000,(double)_ADC::Instance()->Th2o/100,t,self->pyro.sync);
					printf("%4d,%5d,%3.1lf,%hu",ta,(int)tp+0x8000,(double)_ADC::Instance()->Th2o/100,t);
					if(self->ec20.E) {
						//printf(".");								
						if(__time__ > self->timeout) {
							//printf(".");				
							self->ec20.E=0;
						}
					}	else 
						self->timeout = __time__ + 180;
					printf("\r\n");					
				}
//______ print at F8 ___________________________________________________________							
				if(self->pyro.enabled && self->item == EC20) {
					if(self->ec20.E) {							
						if(__time__ > self->timeout) {
							self->Refresh();
							self->ec20.E=0;
						}
					}	else 
						self->timeout = __time__ + 180;	
				}
//______________________________________________________________________________							
#ifdef	USE_LCD
				if(self->plot.Refresh())
					self->lcd.Grid();				
#endif
			}
}
//Q1   +f 0.00229515,0.00459030,0.00229515,1.89738149,-0.90656211
//Q05  +f 0.00219271, 0.00438542, 0.00219271, 1.81269433, -0.82146519

// band +f 0.03207092,0,-0.03207092,1.89866118,-0.93585815
//		  +f 0.16324316,0,-0.16324316,1.64135758,-0.67351367

// high +f 0.98621179,-1.9724235902,0.98621179,1.972233470,-0.97261371

/*
+f 0.15794200,0,-0.15794200,1.65422610,-0.68411599									bp, 10hz

+f 0.02570835,0.05141670,0.02570835,1.35864700,-0.46148042					lp. 10hz
+f 0.00008735,0.00017470,0.00008735,1.96261474,-0.96296415					lp 1Hz
+f 0.00203203,0.00406407,0.00203203,1.81968752,-0.82781567					lp 5 Hz


+f 1.0, 0.00019998012,-0.999800019877, 1.97576083265,  -0.975764811473
+f 1.0,-1.94962512491, 0.949862985499, 1.26448606693,  -0.310384244533
+f 68.4272224754, -112.860738664, 46.3531200753, 0.0,   0.0

+f 1.0, 0.00019998013,-0.999800019866, 1.97163751419,  -0.971642174499
+f 1.0,-1.95436818551, 0.954558994233, 1.26044681223,  -0.309037826299
+f 132.819471678, -224.031891995, 94.1796821905, 0.0,   0.0

*/
