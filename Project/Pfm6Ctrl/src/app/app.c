/**
  ******************************************************************************
  * @file    app.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 Main PFM6 application functionality
  *
  */
	
/** @addtogroup PFM6_Application
* @{
*/
#include	"pfm.h"
#include	<string.h>
#include 	<stdarg.h>
#include 	<stdio.h>

PFM				*pfm;
int				_I1off=0,_I2off=0,
					_U1off=0,_U2off=0,
					_E1ref=0,_E2ref=0;
/*______________________________________________________________________________
* Function Name : App_Init
* Description   : Initialize PFM object
* Input         : None
* Output        : None
* Return        : None
*/
void 			App_Init(void) {
RCC_AHB1PeriphClockCmd(	
					RCC_AHB1Periph_GPIOA |
					RCC_AHB1Periph_GPIOB |
					RCC_AHB1Periph_GPIOC | 
					RCC_AHB1Periph_GPIOD | 
					RCC_AHB1Periph_GPIOE |
					RCC_AHB1Periph_GPIOF |
					RCC_AHB1Periph_GPIOG, ENABLE);

					pfm=calloc(1,sizeof(PFM));
					pfm->Burst.E=0;								
					pfm->Burst.U=0;
					pfm->Burst.HVo=0;
					pfm->Burst.Time=100;
					pfm->Burst.Delay=100;
					pfm->Burst.N=1;
					pfm->Burst.Length=pfm->Burst.Einterval=3000;
					pfm->Burst.Repeat=1000;
					pfm->Burst.Count=1;
					pfm->Burst.Isimm=0;
					pfm->Burst.Idelay=0;
					pfm->Burst.Imax=0;
					pfm->Burst.Psimm[0]=pfm->Burst.Psimm[1]=200*_uS/1000;
					pfm->Burst.LowSimm[0]=pfm->Burst.LowSimm[1]=50*_uS;
					pfm->Burst.LowSimmerMode=_XLAP_QUAD;
					pfm->Burst.HighSimmerMode=_XLAP_QUAD;
					pfm->Burst.Pdelay=pfm->Burst.Pmax=_PWM_RATE_HI*0.02;
					pfm->ADCRate=_uS;
					
					pfm->Pockels.delay=0;
					pfm->Pockels.width=0;
					pfm->Pockels.trigger=0;

					Initialize_NVIC();
					__com0=Initialize_USART(921600);			
					__can=Initialize_CAN(0);	
					Initialize_ADC();
					Initialize_TIM();

#if  			defined (__PFM6__)
					__charger6=Initialize_I2C(0x58,50000);
#elif  		defined (__DISCO__)
{
int				i;
					for(i=0; i<ADC3_AVG; ++i) {
						ADC3_buf[i].HV=ADC3_buf[i].HV2=_HV2AD(700)/ADC3_AVG;
						ADC3_buf[i].IgbtT1=ADC3_buf[i].IgbtT2=0x7ff;
						ADC3_buf[i].Um5=_m5V2AD(-4)/8;
						ADC3_buf[i].Up20=_p20V2AD(20)/8;
					}
}
#else
	#### 		error, no HW defined
#endif
					SysTick_init();
					SetSimmerRate(pfm,_SIMMER_LOW);
					SetPwmTab(pfm);
					Watchdog_init(300);	
					Initialize_DAC();

					_stdio(__com0);
					SystemCoreClockUpdate();
					if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)
						__print("\r ... SWR reset, %dMHz\r\n>",SystemCoreClock/1000000);
					else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
						__print("\r ... IWDG reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)
						__print("\r ... WWDG reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)
						__print("\r ... power on reset\r\n>");
					else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)
					{} else
						{}
					RCC_ClearFlag();   
							
					_batch("cfg.ini");
//					_stdio(NULL);
}
/*______________________________________________________________________________
  * @brief	ISR events polling, main loop
  * @param	PFM object
  * @retval	: None
  */
void			ProcessingEvents(PFM *p) {
//______________________________________________________________________________
static int
					trigger_time=0,
					trigger_count=0;
//_______Fan tacho processing context___________________________________________
{
static 
	int			LastTachoEvent=0,
					TachoEventDivider=0;

					if(_EVENT(p,_FAN_TACHO)) {															// fan timeout counter reset
						_CLEAR_EVENT(p,_FAN_TACHO);
						LastTachoEvent = __time__;
						if(++TachoEventDivider % 5 == 0)
							_BLUE2(20);
					}
#ifndef __DISCO__
					if((__time__ - LastTachoEvent > 200) && (__time__ > 10000))
						_SET_ERROR(p,PFM_FAN_ERR);		
					else
						_CLEAR_ERROR(p,PFM_FAN_ERR);
#endif
}
//
//________processing timed trigger______________________________________________
//
					if(_EVENT(p,_TRIGGER)) {																// trigger request
						_CLEAR_EVENT(p,_TRIGGER);
						if((p->Error & _CRITICAL_ERR_MASK) || trigger_count)	// if error, trigger mot allowed
							trigger_count=0;																		// if trigger_time set (multiple triggers), switch it off
						else {
							trigger_count =  p->Burst.Count;
							trigger_time = __time__;
							if(trigger_count > 1)
								++trigger_time;																		// rearm counters, rounded to next milliseconds to avoid 1ms jitter !!!
						}
					}
//______________________________________________________________________________
					if(trigger_count && __time__ >= trigger_time) {
						Trigger(p);
						if(!_MODE(p,_TRIGGER_PERIODIC))
							--trigger_count;
						trigger_time = __time__ + p->Burst.Repeat;						// rearm counters		
					}
//______________________________________________________________________________
					if(_EVENT(p,_PULSE_FINISHED)) {	
						_CLEAR_EVENT(p,_PULSE_FINISHED);											// end of pulse
						SetSimmerRate(p,_SIMMER_LOW);													// reduce simmer
						if(Eack(p)) {																					// Energ. integrator finished
							Pref1=Pref2=0;
							ScopeDumpBinary(NULL,0);														// scope printout, for testing(if enabled ?)
						}
					}
//______________________________________________________________________________
//					if(_EVENT(p,_ADC_FINISHED)) {	
//						_CLEAR_EVENT(p,_ADC_FINISHED);												// end of pulse
//						_DEBUG_MSG("adc finished...");
//					}
//					if(_E1ref || _E2ref) {
//						_DEBUG_MSG("%d,%d\r\n",_E1ref,_E2ref);
//						_E1ref=_E2ref=0;
//					}
//					if(!_MODE(p,_PULSE_INPROC))
//						IncrementSimmerRate(0);	
}
/*______________________________________________________________________________
  * @brief	periodic status/error  polling, main loop call from 1 msec event flag
  * @param  : current PFM object
  * @retval : None
  *
______________________________________________________________________________*/
void			ProcessingStatus(PFM *p) {
int 			i,j,k;
static
	short		status_image=0; 
static
	int			error_image=0,
					bounce=0;
					
					for(i=j=k=0; i<ADC3_AVG; ++i) {
						j+=ADC3_buf[i].HV;
						k+=ADC3_buf[i].HV2;
					}
					p->HV=j;
					
					p->HV2=k;
					p->Up20 += (8*(ADC3_buf[0].Up20) - p->Up20)/8;
					p->Um5  += (8*(ADC3_buf[0].Um5)  - p->Um5)/8;
//-------------------------------------------------------------------------------			
					p->Temp=IgbtTemp();
					if((p->Temp > (fanTH+fanTH/2)/100) || (p->Temp < -20))
						_SET_ERROR(p,PFM_ERR_TEMP);
					else
						_CLEAR_ERROR(p,PFM_ERR_TEMP);
//-------------------------------------------------------------------------------
					if(p->Up20 < _p20V2AD(18) || p->Up20 > _p20V2AD(22))                      
						_SET_ERROR(p,PFM_ERR_48V);
					else
						_CLEAR_ERROR(p,PFM_ERR_48V);
//-------------------------------------------------------------------------------
					if(p->Um5 > _m5V2AD(-4) || p->Um5 < _m5V2AD(-6))       
						_SET_ERROR(p,PFM_ERR_15V);
					else
						_CLEAR_ERROR(p,PFM_ERR_15V);
//-------------------------------------------------------------------------------
					if(ADC3_buf[0].HV > 100 && abs(ADC3_buf[0].HV-ADC3_buf[0].HV2) > ADC3_buf[0].HV/5)
						_SET_ERROR(p,PFM_HV2_ERR);
					else
						_CLEAR_ERROR(p,PFM_HV2_ERR);
//-------------------------------------------------------------------------------
					if(_STATUS(p,PFM_STAT_SIMM1))	{
						if(TIM_GetITStatus(TIM1, TIM_IT_Update)==RESET) { 
							if(abs(ADC3_buf[0].HV - ADC1_simmer.U) < ADC3_buf[0].HV/8)
								_SET_ERROR(p,PFM_ERR_SIMM1);
							else
								_CLEAR_ERROR(p,PFM_ERR_SIMM1);
						}
					}
//-------------------------------------------------------------------------------
					if(_STATUS(p,PFM_STAT_SIMM2)) {
						if(TIM_GetITStatus(TIM1, TIM_IT_Update)==RESET) {
							if(abs(ADC3_buf[0].HV - ADC2_simmer.U) < ADC3_buf[0].HV/8)
								_SET_ERROR(p,PFM_ERR_SIMM2);
							else
								_CLEAR_ERROR(p,PFM_ERR_SIMM2);
						}					
					}
//-------------------------------------------------------------------------------			
					if((status_image != p->Status) || (error_image != p->Error)) {
						error_image = p->Error;	
						status_image = p->Status;
						bounce=25;
					}
//-------------------------------------------------------------------------------
					_led(-1,-1);
					k=PFM_command(NULL,1);	
					if(bounce && !--bounce) 
						PFM_status_send(p,k);
}		
/*______________________________________________________________________________
  * @brief	Charger6 control procedure; Disables Charger6 if PFM_ERR_DRVERR,PFM_ERR_PULSEENABLE or 
	* _PFM_ADCWDG_ERR are set (from interrupts). Sets the PFM_STAT_PSRDY status bit, 
	* 
  * @param  : current PFM object
  * @retval : None
  *
______________________________________________________________________________*/
void			ProcessingCharger(PFM *p) {
static
	int			ton=1500,					// _PFC_ON command delay
					toff=1000,				// _PFC_OFF command delay
					terr=0,						// shutdown repetition timer
					tpoll=10000;			// 10Hz pfc status polling timer
//-------------------------------------------------------------------------------
// status polling context
//
					if(__time__ >= tpoll) {
						tpoll = __time__ + 100;
						if(_ERROR(p,PFM_I2C_ERR))
							_CLEAR_ERROR(p,PFM_I2C_ERR);									// clear any previous i2c error 
						else {																					// 100 ms charger6 scan, stop when i2c comms error !
int						i=_STATUS_WORD;
							if(readI2C(__charger6,(char *)&i,2))					// add status word >> error status. byte 3
								p->Error = (p->Error & 0xffff) | ((i & 0xff)<<16);
						}
					}
//-------------------------------------------------------------------------------						
//	critical PFM error handling
//
					if(p->Error  & _CRITICAL_ERR_MASK) {
						if(!terr--) {																		// elapsed ?
							int i=_PFC_OFF;																// PFC off
							writeI2C(__charger6,(char *)&i,2);	
							ADC_ITConfig(ADC3,ADC_IT_AWD,DISABLE);	
							terr=500;																			// nest handler delay
							ton=300;																			// recovery delay
							_RED2(100);																		// indicator !!!
							if(PFM_command(NULL,0))												// on error = simmer off
								PFM_command(p,0);
						}
						return;
					}
//-------------------------------------------------------------------------------
					terr=0;																						// clear pending handler
					if(p->Error)																			// non crirical error indicator
						_RED2(100);
//-------------------------------------------------------------------------------
					if(abs(p->HV - p->Burst.HVo) < _HV2AD(50.0))	{		// HV +/- 50V limits !!!
						_SET_STATUS(p,PFM_STAT_PSRDY);
						ADC_ClearITPendingBit(ADC3, ADC_IT_AWD);				// enable HW voltage watchdog
						ADC_ITConfig(ADC3,ADC_IT_AWD,ENABLE);					
						_GREEN2(100);																		// PSREADY indicator 
						toff=0;																					// clear any pending PFC off events
					} else {
						_CLEAR_STATUS(p,PFM_STAT_PSRDY);
						if(!ton && !toff) {															// skip. if contdown running
							if(p->HV > p->Burst.HVo) {										// set countdown on output overshot...
								toff=300;
								ton=3000;
							} else {																			// set countdown on output too low...
								toff=2700;
								ton=3000;
							}
						}
					}	
											
					if(ton) {
						if(!--ton) {																		// switch on countdown
							int i=_PFC_ON;
							writeI2C(__charger6,(char *)&i,2);	
						}		
						if(ton==10)																			// load output voltage 10 ms prior to switch-on
							SetChargerVoltage(_AD2HV(pfm->Burst.HVo));
					}
						
					if(toff)
						if(!--toff) {																		// switch off countdown
							int i=_PFC_OFF;
							writeI2C(__charger6,(char *)&i,2);	
						}			
}
//______________________________________________________________________________________
void			ParseCom(_io *v) {
char 			*p;
int 			i;
_io				*io;

					if(v) {
						if(!v->arg.parse)															// first call init
							v->arg.parse=DecodeCom;
						io=_stdio(v);																	// recursion lock
						i=fgetc(&__stdin);
						switch(i) {
							case _Eof:																	// empty usart
								break;
							case _CtrlZ:																// call watchdog reset
								while(1);
							case _CtrlY:																// call system reset
								NVIC_SystemReset();
							case _Esc:
								_SET_EVENT(pfm,_TRIGGER);									// console esc +-	trigger... no ja!!
								break;
							default:
								p=cgets(i,EOF);
								if(p) {
									while(*p==' ') ++p;
									i=v->arg.parse(p);
									if(*p && i)
										__print("... WTF(%d)",i);							// error message
									v->arg.parse(NULL);											// call newline
								}
						}
						_stdio(io);
					}
}
/*______________________________________________________________________________
  * @brief  CAN transmit parser
  * @param:
  * @retval : None
	*
	*
  */
void			ParseCanTx(PFM *p) {	
CanTxMsg	tx={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};

					while((__CAN__->TSR & CAN_TSR_TME)) {
						if(_buffer_pull(__can->tx,&tx,sizeof(CanTxMsg)))	
							CAN_Transmit(__CAN__,&tx);
						else if(__can->arg.io) {
							tx.DLC=_buffer_pull(__can->arg.io->tx,tx.Data,8);
							if(tx.DLC > 0) {
								tx.StdId=_ID_PFMcom2SYS;
								CAN_Transmit(__CAN__,&tx);
							} else
								break;
						} else
							break;
						
						if(_DBG(pfm,_DBG_CAN_TX) && tx.StdId) {
							_io *io=_stdio(__dbug);
							int i;
							__print(":%04d >%02X ",__time__ % 10000,tx.StdId);
							for(i=0;i<tx.DLC;++i)
								__print(" %02X",tx.Data[i]);
							__print("\r\n>");
							_stdio(io);
						}
					}			
}					
/*______________________________________________________________________________
  * @brief  CAN message parser for PFM data as defined in CAN protocol  ICD 
  * @param rx: pointer to CanRxMsg structure
  * @retval : None
	*
	*
  */
void			ParseCanRx(PFM *p) {	
CanRxMsg	rx;
short			n;
char			*q=(char *)rx.Data;
//______________________________________________________________________________________					
					if(_buffer_pull(__can->rx,&rx,sizeof(CanRxMsg))) {
						q=(char *)rx.Data;
//______________________________________________________________________________________
						if(_DBG(p,_DBG_CAN_RX))
						{
							_io *io=_stdio(__dbug);
							__print(":%04d <%02X ",__time__ % 10000,rx.StdId);
							for(n=0;n<rx.DLC;++n)
								__print(" %02X",rx.Data[n]);
							__print("\r\n>");
							_stdio(io);
						}
//______________________________________________________________________________________
						switch(rx.StdId) {
							case _ID_SYS_TRIGG:
								_SET_EVENT(p,_TRIGGER);
							break;				
//______________________________________________________________________________________
							case _ID_PFMcom2SYS:
								if(_DBG(p,_DBG_CAN_COM))	{
									_io *io=_stdio(__dbug);
									for(n=0;n<rx.DLC;++n)
										fputc(rx.Data[n],&__stdout);
									_stdio(io);
								}
								break;
//______________________________________________________________________________________
							case _ID_SYS2PFMcom:
								if(rx.DLC) {
									if(__can->arg.io == NULL) {
										__can->arg.io=_io_init(128,128);
										App_Add((func *)ParseCom,(arg*)&__can->arg.io,"ParseCAN-IO",0);
									}
									while(__can->arg.io->rx->size - _buffer_len(__can->arg.io->rx) < 8)
										Wait(2,App_Loop);
									_buffer_push(__can->arg.io->rx,rx.Data,rx.DLC);
								} else {
									__can->arg.io=_io_close(__can->arg.io);
									App_Remove((func *)ParseCom,(arg*)&__can->arg.io);
								}
								break;												
//______________________________________________________________________________________
							case _ID_SYS2PFM:
								switch(*(uint8_t *)q++) {
									case _PFM_status_req:
										PFM_status_send(p,PFM_command(NULL,0));
										break;
									case _PFM_IgbtTemp_req:
										CanReply("ccP",_PFM_IgbtTemp_ack,p->Temp);
										break;
									case _PFM_U_req:
										CanReply("cwP",_PFM_U_req,p->Burst.U);
										break;
									case _PFM_command:
										PFM_command(p,rx.Data[1]);
										Eack(NULL);
										break;
									case _PFM_set:
										p->Burst.U = *(short *)q++;q++;
										p->Burst.Time=*(short *)q++;q++;
										p->Burst.Ereq=*q++;
// __________________________________________________________________________________________________________
										p->Burst.Pmax = __max(0,__min(_PWM_RATE_HI, (p->Burst.U *_PWM_RATE_HI)/_AD2HV(10*p->Burst.HVo)));
// __________________________________________________________________________________________________________
										if(p->Burst.Pmax > 0 && p->Burst.Pmax < _PWM_RATE_HI) {
											p->Burst.Imax=__min(4095,_I2AD(p->Burst.U/10 + p->Burst.U/2));
											SetPwmTab(p);														
											Eack(NULL);
										} else {
											_SET_ERROR(p,PFM_ERR_PSRDYN);
										}
										break;
// __________________________________________________________________________________________________________
									case _PFM_reset:
										p->Burst.Repeat=*(short *)q++;q++;
										p->Burst.N=*q++;
										p->Burst.Length=*q++*1000;
										p->Burst.E=*(short *)q;
										p->Burst.Count=1;
// ________________________________________________________________smafu za preverjanje LW protokola ________
										if(p->Burst.N==0)
											p->Burst.N=1;
										if(p->Burst.Length==0)
											p->Burst.Length=3000;	
										p->Burst.Erpt = 0;
// __________________________________________________________________________________________________________
										if(_MODE(p,_LONG_INTERVAL)) {
											for(n=0; n<8; ++n)
												if((_ADCRates[n]+12)*(_MAX_BURST/_uS)/15 >  p->Burst.Length)
													break;
													
											ADC_RegularChannelConfig(ADC1, ADC_Channel_8,		1, n);
											ADC_RegularChannelConfig(ADC1, ADC_Channel_2,		2, n);
											ADC_RegularChannelConfig(ADC2, ADC_Channel_11,	1, n);
											ADC_RegularChannelConfig(ADC2, ADC_Channel_12,	2, n);
											p->ADCRate=((_ADCRates[n]+12)*_uS)/15;											
//___________________________________________________________________________________________________________								
										} else {																															// NdYag long pulse burst, LW 4x2ms, 15/25 ms burst 
											if(p->Burst.Length > _MAX_BURST/_uS) {
												p->Burst.Repeat = (p->Burst.Length / p->Burst.N + 500)/1000;			// repetition rate = burst repetition, rounded to 1ms
												p->Burst.Erpt = p->Burst.N-1;																			// energy report after N pulses
												p->Burst.Length=(p->Burst.Repeat-1)*1000;													// burst length = repetition(us) - 1ms
												p->Burst.N=1;																											// treated as single pulse
											}
										}
//___________________________________________________________________________________________________________								
										SetPwmTab(p);
										Eack(NULL);
										break;
									case _PFM_simmer_set:
										p->Burst.Psimm[0]=*(short *)q/50 + 7;
										++q;++q;
										p->Burst.Psimm[1]=*(short *)q/50 + 7;
										SetSimmerRate(p,_SIMMER_LOW);					
										break;
									case _PFM_RevNum_req:
										n=*(short *)q;
										CanReply("cwwP",_PFM_RevNum_req,SW_version,n);
										break;
									case _PFM_Ping:
										CanReply("cP",_PFM_Ping);
										break;
									case _PFM_Iap:
										while(1);
//___________________________________________________________________________________________________________								
//
// Pfm6 add..
//
// ________________________________________________________________________________
									case _PFM_POCKELS: 																					// 0x73, _PFM_POCKELS #ghdg78236u
										p->Pockels.delay=*(short *)q++;q++;
										p->Pockels.width=*(short *)q++;q++;
										PFM_pockels(p);
										break;

									case _PFM_SetHVmode:																				// 0x72, _PFM_SetHVmode 
									{																														// HV & mode configuration modif. by host
										char c[16];
										sprintf(c,"u %d",*(short *)q);
										if(DecodeCom(c) == _PARSE_OK) {
											_CLEAR_ERROR(p,PFM_ERR_UB);
											++q;++q;
											if(*q & 1)
												_CLEAR_MODE(p,_CHANNEL1_DISABLE);
											else
												_SET_MODE(p,_CHANNEL1_DISABLE);
											if(*q & 2)
												_CLEAR_MODE(p,_CHANNEL1_SINGLE_TRIGGER);
											else
												_SET_MODE(p,_CHANNEL1_SINGLE_TRIGGER);
											if(*q & 4)
												_CLEAR_MODE(p,_CHANNEL2_DISABLE);
											else
												_SET_MODE(p,_CHANNEL2_DISABLE);
											if(*q & 8)
												_CLEAR_MODE(p,_CHANNEL2_SINGLE_TRIGGER);
											else
												_SET_MODE(p,_CHANNEL2_SINGLE_TRIGGER);
										} else
											_SET_ERROR(p,PFM_ERR_UB);
									}
									break;

									case _PFM_CurrentLimit:																			// .10
									{
										int dac1,dac2;
										dac1=*(short *)q;
										++q;++q;
										dac2=*(short *)q;
										DAC_SetDualChannelData(DAC_Align_12b_R,dac1,dac2);
										DAC_DualSoftwareTriggerCmd(ENABLE);									
									}
										break;	
									case _PFM_HV_req:																						// .12
										CanReply("cwwwP",_PFM_HV_req,p->HV,p->HV2,p->Temp);
										break;	
									case _PFM_TRIGG:																						// .71
										_SET_EVENT(p,_TRIGGER);
										break;
								}
								break;
//______________________________________________________________________________________
								case _ID_ENRG2SYS: 																						// energometer message 
								{
									union {short w[4];} *e = (void *)q; 
									if(_DBG(p,_DBG_MSG_ENG) && (unsigned short)e->w[0]==0xD103) {
										_io *io=_stdio(__dbug);				
										__print(":%04d e1=%.1lf,e2=%.1lf\r\n>",__time__ % 10000, 
											(double)__max(0,e->w[2])/10,
												(double)__max(0,e->w[3])/10);
										_stdio(io);
									}
								}
								break;
//______________________________________________________________________________________
								case _ID_SYS2EC:
								switch(*q++) {
									case _EC_status_req:
										CanReply("cwwE",_EC_status_req,0,0);
										break;
									case _EC_command:
										CanReply("cwwE",_EC_status_req,0,0);
										break;
									case _EC_RevNum_req:
										n=*(short *)q;
										CanReply("cwwE",_EC_RevNum_req,SW_version,n);
										break;
									case _EC_Ping:
										CanReply("cE",_EC_Ping);
										break;
								}
								break;
//______________________________________________________________________________________
							}
						}
}
/*______________________________________________________________________________
  * @brief  CAN data transmission reply
  * @param format: message format string
  * @param ....  : list of parameters, acc. to format specifier
  * @retval : None
  */
int				CanReply(char *format, ...) {
static 
_io*			io=NULL;
union			{
						void *v;
						int i;
						char c[8];
					} u;
CanTxMsg	tx={_ID_PFM2SYS,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};
va_list		v;
char*			c;
int				i;				
					if(!format) {
						io=__stdin.io;
						return(0);
					}
					va_start(v, format);
					while(*format) { 
						u.i=va_arg(v, int);
						c=u.c;
						i=0;
						switch(*format++) {
							case '.': memcpy(&tx,u.v,sizeof(tx));	break;							
							case 'P':	tx.StdId=_ID_PFM2SYS;			break;
							case 'E':	tx.StdId=_ID_EC2SYS;			break;
							case 'X':	tx.StdId=*c++;						break;
							case 'c':	i=sizeof(char);						break;
							case 'w':	i=sizeof(short);					break;
							case 'i':	i=sizeof(int);						break;
						}
						while(i--)			
							tx.Data[tx.DLC++]=*c++;
					}
					va_end(v);	
//______________________________________________________________________________________
					if(io) {
						io=_stdio(io);
						__print("%02X.<<.",tx.StdId);
						for(i=0; i<tx.DLC; ++i)
							__print("%c%02X",'.',tx.Data[i]);
						__print("\r\n>");	
						io=_stdio(io);						
					}

//					CAN_ITConfig(__CAN__, CAN_IT_TME, DISABLE);	
					i=_buffer_push(__can->tx,&tx,sizeof(CanTxMsg));
//					CAN_ITConfig(__CAN__, CAN_IT_TME, ENABLE);	
					return i;
}
/*______________________________________________________________________________
  * @brief  Interprets the PFM command message
  * @param 	pfmcmd: PFM command word as defined in CAN protocol  ICD 
  * @retval : None
  *
  */
#define		kVf	(3.3/4096.0*2000.0/7.5)					// 		flash voltage			
#define		kIf	(3.3/4096.0/2.9999/0.001)				// 		flash curr.
#define 	Ts	 1e-6														// 		ADC sample rate
#define 	kmJ	(int)(0.001/kVf/kIf/Ts+0.5) 		//		mJ, fakt. delitve kum. energije < 1 !!!  0.4865351
	
int				Eack(PFM *p) {

static		long long	e1=0,e2=0;
static		int				n=0;

int				i,j,k;

					if(p) {
						j=k=_I2AD(20.0);
						for(i=0; i < p->Burst.Einterval*_uS/_MAX_ADC_RATE ; ++i) {
							if(ADC1_buf[i].I > j)
								e1+=(short)(ADC1_buf[i].U) * (short)(ADC1_buf[i].I-_I1off);
							if(ADC2_buf[i].I > k)
								e2+=(short)(ADC2_buf[i].U) * (short)(ADC2_buf[i].I-_I2off);							
						}
						
						if(n++ == p->Burst.Erpt) {
							int e;
							if(_STATUS(p,PFM_STAT_SIMM1) && !_STATUS(p,PFM_STAT_SIMM2)) {
								e=e1*p->ADCRate/kmJ/_uS;
								CanReply("cicP",_PFM_E_ack,e,0);	
								_DEBUG_MSG("E1=%d.%dJ",e/1000,(e%1000)/100);
							}
							if(!_STATUS(p,PFM_STAT_SIMM1) && _STATUS(p,PFM_STAT_SIMM2)) {
								e=e2*p->ADCRate/kmJ/_uS;
								CanReply("cicP",_PFM_E_ack,e,0);					
								_DEBUG_MSG("E2=%d.%dJ",e/1000,(e%1000)/100);
							}
							if(_STATUS(p,PFM_STAT_SIMM1) && _STATUS(p,PFM_STAT_SIMM2)) {
								e=(e1+e2)*p->ADCRate/kmJ/_uS;
								CanReply("cicP",_PFM_E_ack,e,0);		
								_DEBUG_MSG("E1+E2=%d.%dJ",e/1000,(e%1000)/100);
							}
							e1=e2=n=0;
							return(-1);
						}
					} else
						e1=e2=n=0;
					return(0);
}
/*______________________________________________________________________________
  * @brief  Composes & sends the PFM status message
  * @param  : PFM object p, simmer status k
  * @retval : None
  *
  */
int				PFM_status_send(PFM *p, int k) {
						if(p->Error & (PFM_STAT_SIMM1 | PFM_STAT_SIMM2))
							CanReply("cwiP",_PFM_status_req,
								(p->Status & ~(PFM_STAT_SIMM1 | PFM_STAT_SIMM2)) | k,
								(p->Error & ~(PFM_STAT_SIMM1 | PFM_STAT_SIMM2)) | k
								);
						else
							CanReply("cwiP",_PFM_status_req,
								(p->Status & ~(PFM_STAT_SIMM1 | PFM_STAT_SIMM2)) | k,
								p->Error);		
					return k;
					}
/*______________________________________________________________________________
  * @brief  Interprets the PFM command message
  * @param 	pfmcmd: PFM command word as defined in CAN protocol  ICD 
  * @retval : None
  *
  */
//					p 1000,400,0,1200
//					b 1,1200,330,3
					
int				PFM_command(PFM *p, int n) {
static		int	count=0,no=0;
					int	Uidle;
//________________________________________________________________________________
					if(p) {
						while(_MODE(p,_PULSE_INPROC))																			// no change during pulse
							Wait(2,App_Loop);
//________________________________________________________________________________
						if(no != n) {																											// simmer status changed ???
							_TRIGGER1_OFF;																									// kill both triggers
							_TRIGGER2_OFF;
							_CLEAR_STATUS(p,PFM_STAT_SIMM1);																// clear status
							_CLEAR_STATUS(p,PFM_STAT_SIMM2);
							SetSimmerPw(p);																									// kill both simmers
							no=n & (PFM_STAT_SIMM1 | PFM_STAT_SIMM2);												// mask filter command
							Wait(100,App_Loop);																							// wait 100 msecs
							
							if(!_MODE(p,_CHANNEL1_DISABLE)) {																// if not Erbium  single channel
								if(_MODE(p,_CHANNEL1_SINGLE_TRIGGER))													// single trigger config.. as from V1.11
									Uidle=2*p->HV/7;
								else
									Uidle=p->HV/7;
								_I1off=ADC1_simmer.I;																					// get current sensor offset
								_U1off=ADC1_simmer.U;																					// check idle voltage
#ifndef __DISCO__
								if(abs(Uidle - ADC3_AVG*ADC1_simmer.U) > _HV2AD(30)) {				// HV +/- 30V range ???
									_SET_ERROR(p,PFM_STAT_UBHIGH);															// if not, PFM_STAT_UBHIGH error 
									no=0;
								}
#endif
							}
							if(!_MODE(p,_CHANNEL2_DISABLE)) {																// same for NdYAG channel
								if(_MODE(p,_CHANNEL2_SINGLE_TRIGGER))													// single trigger config.. as from V1.11
									Uidle=2*p->HV/7;
								else
									Uidle=p->HV/7;
								_I2off=ADC2_simmer.I;
								_U2off=ADC2_simmer.U;
#ifndef __DISCO__
								if(abs(Uidle - ADC3_AVG*ADC2_simmer.U) > _HV2AD(30)) {
									_SET_ERROR(p,PFM_STAT_UBHIGH);
									no=0;
								}
#endif
							}
						}
//________________________________________________________________________________
						if(no && _MODE(p,_CHANNEL1_DISABLE))															// if single channel request
							_SET_STATUS(p, PFM_STAT_SIMM2);																	// modify status
						else if(no && _MODE(p,_CHANNEL2_DISABLE))													// same for other channel
							_SET_STATUS(p, PFM_STAT_SIMM1);
						else
							_SET_STATUS(p, no);																							// else set status as requested
//________________________________________________________________________________
#ifndef __DISCO__
						if(!_STATUS(p,_PFM_CWBAR_STAT))																		// crowbar not cleared
							_SET_ERROR(p,PFM_ERR_PULSEENABLE);
#endif
//________________________________________________________________________________
						if(no & PFM_STAT_SIMM1)																						// activate triggers
							_TRIGGER1_ON;
						if(no & PFM_STAT_SIMM2)
							_TRIGGER2_ON;
						
						SetSimmerRate(p,_SIMMER_LOW);																			// set simmer
						SetPwmTab(p);
						count=1000;																												// set trigger countdown
//________________________________________________________________________________					
					} else {
						if(count && !(count -= n)) {																			// #93wefjlnw83
							_TRIGGER1_OFF;																									// kill triggers after count interval
							_TRIGGER2_OFF;
						}
					}						
					return	no;
}
/*______________________________________________________________________________
  * @brief  Pockels cell driver setup
  * @param 	q == can buffer (short delay, short pw, short n-pulses
	* if q==0 >> call from interrupt
  * @retval : None
  *
  */
int				PFM_pockels(PFM *p) {

					TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
					TIM_OCInitTypeDef					TIM_OCInitStructure;
					GPIO_InitTypeDef					GPIO_InitStructure;
					
					TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
					TIM_OCStructInit(&TIM_OCInitStructure);
					GPIO_StructInit(&GPIO_InitStructure);
					TIM_DeInit(TIM4);
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
					
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
					GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
						
					GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
					GPIO_Init(GPIOD, &GPIO_InitStructure);
															
					TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
					TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
					TIM_OCInitStructure.TIM_Pulse=p->Pockels.delay + 1;
					TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
					TIM_OC1Init(TIM4, &TIM_OCInitStructure);		
					
					TIM_TimeBaseStructure.TIM_Period = p->Pockels.delay + p->Pockels.width + 1;
					TIM_TimeBaseStructure.TIM_Prescaler = _uS/10-1;
					TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
					TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
					
					TIM_Cmd(TIM4, DISABLE);
					return 0;
}
/**
* @}
*/ 
