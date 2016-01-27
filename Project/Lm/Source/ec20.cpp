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

#include	"isr.h"
#include	"ec20.h"
#include	"lm.h"
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_EC20::_EC20() {
	repeat=2;
	pw=500;
	width=200;
	Uo=400;
	Fo=100;
	idx=mode=status=error=E=0;
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
				sscanf(c,"%d,%d,%d",&pw,&width,&repeat);
}
/*******************************************************************************/
/**
	* @param	: None
	* @brief	SaveSettings
	* @retval : None
	*/
void		_EC20::SaveSettings(FILE *f) {
				fprintf(f,"%5d,%5d,%5d       /.. flash\r\n",pw,width,repeat);
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
#if defined (__DISCO__) || defined (__IOC_V1__)
int			_EC20::Increment(int updown, int leftright, void *parent) {
				_LM 		*lm = static_cast<_LM *>(parent);	
				CanMsg 	msg;
				switch(idx=__min(__max(idx+leftright,0),3)) {
					case 0:
						pw= __min(__max(pw+updown,5),995);
					break;
					case 1: 
						width= __min(__max(width+updown,100),1000);
					break;
					case 2:
						repeat= __min(__max(repeat+updown,2),50);
					break;
					case 3:
						break;
					}
		    
					printf("\r:EC20         %3.1lf%c,%4dus,%4dHz,",((double)pw)/10,'%',width,repeat);
		    
					if(updown || leftright)
						{
							E=0;
							if(idx < 3)
								{
									msg.EC20set.Code=Id_EC20Set;
									msg.EC20set.Uo=Uo*10;
									msg.EC20set.To=width;
									msg.EC20set.Mode=2;																																		 //??
									lm->can.Send(Sys2Ec,&msg,sizeof(EC20set));
		    
									msg.EC20reset.Code=Id_EC20Reset;
									msg.EC20reset.Period=1000/repeat;
									msg.EC20reset.Pw=pw;
									msg.EC20reset.Fo=Fo;
									lm->can.Send(Sys2Ec,&msg,sizeof(EC20reset));
								}
						}
						
// ec20 states as from status....
#define 		_COMPLETED	0x8000
#define			_SIMGEN			0x4000
#define 		_SIM_DET		0x0001		
#define 		_TS1_IOC		0x1000
#define 		_FOOT_ACK		0x0200
#define			_STATUS_MASK				(_COMPLETED +  _SIMGEN +  _SIM_DET + _TS1_IOC +  _FOOT_ACK)
// ec20 		status commands				
#define			_SIMGEN			0x4000
#define			_HV1_EN			0x0001
#define 		_FOOT_REQ		0x0100
						
						msg.EC20com.Code=Id_EC20Com;
						msg.EC20com.Command=0;
						lm->pyro.enabled=true;

						switch(status & _STATUS_MASK) {
							case _COMPLETED:
								msg.EC20com.Command=_SIMGEN;
							break;

							case _COMPLETED + _SIMGEN:											// standby
								printf(" STNDBY");
								if(updown>0 && idx==3)
									msg.EC20com.Command=_SIMGEN + _HV1_EN;
								break;

							case _COMPLETED + _SIMGEN + _SIM_DET:						// simmer
								printf(" SIMMER");
								if(updown>0 && idx==3)
									msg.EC20com.Command=_SIMGEN + _HV1_EN + _FOOT_REQ;
								if(updown<0 && idx==3)
									msg.EC20com.Command=_SIMGEN;
								break;

							case _COMPLETED + _SIMGEN + _SIM_DET + _TS1_IOC:
							case _COMPLETED + _SIMGEN + _SIM_DET + _FOOT_ACK:
							case _COMPLETED + _SIMGEN + _SIM_DET + _TS1_IOC + _FOOT_ACK:
								printf(" LASE..");														// lasing
								if(updown<0 && idx==3)
									msg.EC20com.Command=_SIMGEN + _HV1_EN;
								break;
		    
							default:
								printf(" wait..");														// cakanje na ec20
								break;
							}
		    
							if(msg.EC20com.Command)
								lm->can.Send(Sys2Ec,&msg,sizeof(EC20com));
		    
							char c[128];
							sprintf(c,"  %3.1lfJ,%5dW,%3.1lf'C,%3.1lf'C,%5.1lf",
																													(double)E/1000,
																														E*repeat/1000,
																															(double)_ADC::Instance()->Th2o/100,
																																(lm->plotA-7800.0)/200.0+25.0,
																																	(double)lm->plotB);
																															
							if(E != 0)																			// ce strel, izpis energije, temperature
								printf("%s",c);
							else
								printf("%*c",strlen(c)+1,' ');
							
							for(int i=7*(8-idx)+3; i--; printf("\b"));			// mismas... poravnava kurzorja	xD
							return 0;
	}
#elif defined (__IOC_V2__)
int				_EC20::Increment(int updown, int leftright, void *parent) {
_LM 			*lm = static_cast<_LM *>(parent);		
CanMsg		msg;
char			s[16];

					switch(idx=__min(__max(idx+leftright,0),3)) {
						case 0:
							pw= __min(__max(pw+updown,5),995);
						break;
						case 1: 
							width= __min(__max(width+updown,100),1000);
						break;
						case 2:
							repeat= __min(__max(repeat+updown,2),50);
						break;
						case 3:
							break;
						}
		    
						printf("\r:EC20         %3.1lf%c,%4dus,%4dHz,",((double)pw)/10,'%',width,repeat);
		    
						if(updown || leftright) {
							E=0;
							if(idx < 3)
								{
									msg.EC20set.Code=Id_EC20Set;
									msg.EC20set.Uo=Uo*10;
									msg.EC20set.To=width;
									msg.EC20set.Mode=2;																																		 //??
									lm->can.Send(Sys2Ec,&msg,sizeof(EC20set));
		    
									msg.EC20reset.Code=Id_EC20Reset;
									msg.EC20reset.Period=1000/repeat;
									msg.EC20reset.Pw=pw;
									msg.EC20reset.Fo=Fo;
									lm->can.Send(Sys2Ec,&msg,sizeof(EC20reset));
								}
						}
						
// ec20 states as from status....
#define 		_COMPLETED	0x8000
#define			_SIMGEN			0x4000
#define 		_SIM_DET		0x0001		
#define 		_TS1_IOC		0x1000
#define 		_FOOT_ACK		0x0200
#define			_STATUS_MASK				(_COMPLETED  +  _SIM_DET  +  _FOOT_ACK)
// ec20 		status commands				
#define			_SIMGEN			0x4000
#define			_HV1_EN			0x0001
#define 		_FOOT_REQ		0x0100
#define 		_NOCOMM			0xffff
						
						msg.EC20com.Code=Id_EC20Com;
						msg.EC20com.Command=_NOCOMM;
						lm->pyro.enabled=true;

						switch(status & _STATUS_MASK) {
							case _COMPLETED						:												// standby
								sprintf(s," STNDBY");
								if(updown>0 && idx==3)
									msg.EC20com.Command=_HV1_EN;
								break;

							case _COMPLETED + _SIM_DET:												// simmer
								sprintf(s," SIMMER");
								if(updown>0 && idx==3)
									msg.EC20com.Command=_HV1_EN + _FOOT_REQ;
								if(updown<0 && idx==3)
									msg.EC20com.Command=0;
								break;

							case _COMPLETED  + _SIM_DET + _FOOT_ACK:
								sprintf(s," LASE..");														// lasing
								if(updown<0 && idx==3)
									msg.EC20com.Command=_HV1_EN;
								break;
		    
							default:
								sprintf(s," wait..");														// cakanje na ec20
								break;
							}
							printf("%s",s);
							
							if(msg.EC20com.Command != _NOCOMM)
								lm->can.Send(Sys2Ec,&msg,sizeof(EC20com));
		    
							char c[128];
							sprintf(c,"  %3.1lfJ,%5dW,%3.1lf'C,%3.1lf'C,%5.1lf",
																													(double)E/1000,
																														E*repeat/1000,
																															(double)_ADC::Instance()->Th2o/100,
																																(lm->plotA-7800.0)/200.0+25.0,
																																	(double)lm->plotB);
																															
							if(E != 0)																			// ce strel, izpis energije, temperature
								printf("%s",c);
							else
								printf("%*c",strlen(c)+1,' ');
							
							for(int i=7*(8-idx)+3; i--; printf("\b"));			// mismas... poravnava kurzorja	xD
							return 0;
	}
#else
	***error: HW platform not defined
#endif
/**
* @}
*/ 
