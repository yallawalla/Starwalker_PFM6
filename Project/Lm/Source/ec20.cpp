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

#include	"lm.h"
#include	"ec20.h"
#include	<math.h>
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_EC20::_EC20(void *v) {
	parent = v;
	idx=timeout=0;
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_EC20::~_EC20() {
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void		_EC20::LoadSettings(FILE *f) {
char		c[128];
				fgets(c,sizeof(c),f);
				sscanf(c,"%hu,%hu,%hu",&EC20Reset.Pw,&EC20Set.To,&EC20Reset.Period);
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
void		_EC20::SaveSettings(FILE *f) {
				fprintf(f,"%5d,%5d,%5d       /.. flash\r\n",EC20Reset.Pw,EC20Set.To,EC20Reset.Period);
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
int				_EC20::Increment(int updown, int leftright) {
_LM 			*lm = static_cast<_LM *>(parent);		
_EC20Cmd		m;
char			s[16];

					switch(idx=__min(__max(idx+leftright,0),3)) {
						case 0:			
							EC20Reset.Pw		= __min(__max(EC20Reset.Pw+updown,5),995);
						break;
						case 1: 
							EC20Set.To			= __min(__max(EC20Set.To+updown,100),1000);
						break;
						case 2:
							EC20Reset.Period= __min(__max(EC20Reset.Period+updown,2),50);
						break;
						case 3:
							break;
					}
		    
					printf("\r:EC20         %3.1lf%c,%4dus,%4dHz,",((double)EC20Reset.Pw)/10,'%', EC20Set.To, EC20Reset.Period);
	    
					if(updown || leftright) {
						EC20Eo.C=0;
						if(idx < 3) {
_EC20Set			p=EC20Set;
_EC20Reset		q=EC20Reset;
							p.Uo = 10*EC20Set.Uo;
							q.Period=1000/EC20Reset.Period;
							p.Send(Sys2Ec);
							q.Send(Sys2Ec);
						}
					}
					
					lm->pyro.Enabled=true;
					Timeout(EOF);
					switch(EC20Status.Status & _STATUS_MASK) {
						case _COMPLETED:																	// standby
							sprintf(s," STNDBY");
							lm->pilot.Off();
							lm->Submit("@standby.led");
							if(updown>0 && idx==3)
								m.Cmd=_HV1_EN;
						break;
							
						case _COMPLETED + _SIM_DET:												// simmer
							sprintf(s," SIMMER");
							lm->pilot.On();
							lm->Submit("@ready.led");
							if(updown>0 && idx==3)
								m.Cmd =_HV1_EN + _FOOT_REQ;
							if(updown<0 && idx==3)
								m.Cmd =0;
						break;
							
						case _COMPLETED  + _SIM_DET + _FOOT_ACK:
							sprintf(s," LASE..");														// lasing
							lm->pilot.On();
							lm->Submit("@lase.led");
							if(updown<0 && idx==3)
								m.Cmd =_HV1_EN;
						break;

						default:
							lm->Submit("@standby.led");
							sprintf(s," wait..");														// cakanje na ec20
							lm->pilot.Off();
								m.Cmd =0;
							break;
						}
					
						printf("%s",s);

						if(m.Cmd != _NOCOMM) {
							m.Send(Sys2Ec);
							Timeout(500);
						}

						char c[128];
						sprintf(c,"  %3.1lfJ,%5dW,%3.1lf'C,%3.1lf'C,%5.1lf",
																												(double)EC20Eo.C/1000,
																													EC20Eo.C*EC20Reset.Period/1000,
																														(double)_ADC::Th2o()/100,
																															(lm->plotA-7800.0)/200.0+25.0,
																																(double)lm->plotB);
																														
						if(EC20Eo.C != 0)																	// ce strel, izpis energije, temperature
							printf("%s",c);
						else
							printf("%*c",strlen(c)+1,' ');
						
						for(int i=7*(8-idx)+3; i--; printf("\b"));				// mismas... poravnava kurzorja	xD
						return 0;
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
void				_EC20::Parse(CanTxMsg	*msg) {
_LM 				*lm = static_cast<_LM *>(parent);	

						switch(msg->StdId) {
/******************************************************************************/	
//____________Sys to EC20 message ______________________________________________
/******************************************************************************/	
							case Sys2Ec:
								switch(*msg->Data) {
//____________Sys to EC20 status req, only for debugging________________________________
									case Id_EC20Status:
										if(_BIT(_LM::debug, DBG_EC_SIM)) {
											EC20Status.Status= _COMPLETED;
											EC20Status.Send(Ec2Sys);
									}
									break;
//____________Sys to EC20 Uo, To, mode__________________________________________________
									case Id_EC20Set:
										memcpy(&EC20Set, msg->Data, msg->DLC);
										EC20Set.Uo /= 10;
									break;
//____________Sys to EC20 repetition, PW, fo ___________________________________________
									case Id_EC20Reset:
										memcpy(&EC20Reset, msg->Data, msg->DLC);
										EC20Reset.Period = 1000/EC20Reset.Period;
									break;
//____________Sys to EC20 command, only for debugging___________________________________
									case Id_EC20Cmd:
										memcpy(&EC20Cmd, msg->Data, msg->DLC);
										switch(EC20Cmd.Cmd) {
											case _HV1_EN:
												if(_BIT(_LM::debug, DBG_EC_SIM)) {
													EC20Status.Status=_COMPLETED + _SIM_DET;
													EC20Status.Send(Ec2Sys);
													_thread_remove((void *)ECsimulator,this);
												}
											break;
											case _HV1_EN + _FOOT_REQ:
												if(_BIT(_LM::debug, DBG_EC_SIM)) {
													EC20Status.Status=_COMPLETED  + _SIM_DET + _FOOT_ACK;
													EC20Status.Send(Ec2Sys);
													_thread_add((void *)ECsimulator,this,(char *)"EC20 simulator",1000/EC20Reset.Period);
												}
											break;
											default:
												if(_BIT(_LM::debug, DBG_EC_SIM)) {
													EC20Status.Status=_COMPLETED;
													EC20Status.Send(Ec2Sys);
												}
											break;
										}
									break;
									default:
										break;
								}
								break;
/******************************************************************************/	
//____________EC20 to Sys message ______________________________________________
/******************************************************************************/	
							case Ec2Sys:
								switch(*msg->Data) {
//____________EC20 to Sys status  ______________________________________________________
									case Id_EC20Status:
										memcpy(&EC20Status, msg->Data, msg->DLC);
										if(lm->Selected() == EC20)
											lm->Refresh();
									break;	
//____________EC20 to Sys energy  ______________________________________________________
									case Id_EC20Eo:
										lm->Submit("@energy.led");
										memcpy(&EC20Eo, msg->Data, msg->DLC);
										if(lm->pyro.Enabled && lm->Selected() == EC20)
											lm->Refresh();
									break;
								}
								break;
							}
						}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
void			Send2Can(_stdid std, void *v, size_t n) {
CanTxMsg	m={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};
					m.StdId=std;
					m.DLC=n;
					memcpy(m.Data,v,n);
					_CAN::Instance()->Send(&m);
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
void		_EC20::ECsimulator(void *v) {
_EC20 *ec = static_cast<_EC20 *>(v);
				ec->EC20Eo.C=ec->EC20Eo.UI=pow((double)(ec->EC20Set.Uo * ec->EC20Reset.Pw / 1000),3)/420*ec->EC20Set.To/1000;
				ec->EC20Eo.Send(Ec2Sys);
}
/**
* @}
*/ 
