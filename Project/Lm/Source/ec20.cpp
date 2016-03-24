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

	_EC20status	_EC20::EC20status	=	{Id_EC20Status,0,0};
	_EC20Cmd		_EC20::EC20Cmd 		= {Id_EC20Cmd,0};
	_EC20set		_EC20::EC20set		=	{Id_EC20Set,400,200,0};
	_EC20reset	_EC20::EC20reset	=	{Id_EC20Reset,2,500,100};
	_EC20energy	_EC20::EC20energy	=	{Id_EC20Energy,0,0};
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_EC20::_EC20() {
	idx=0;
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
				sscanf(c,"%hu,%hu,%hu",&EC20reset.Pw,&EC20set.To,&EC20reset.Period);
}
/*******************************************************************************/
/**
	* @param	: None
	* @brief	SaveSettings
	* @retval : None
	*/
void		_EC20::SaveSettings(FILE *f) {
				fprintf(f,"%5d,%5d,%5d       /.. flash\r\n",EC20reset.Pw,EC20set.To,EC20reset.Period);
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
int				_EC20::Increment(int updown, int leftright, void *parent) {
_LM 			*lm = static_cast<_LM *>(parent);		
CanMsg		msg;
char			s[16];

					switch(idx=__min(__max(idx+leftright,0),3)) {
						case 0:
							EC20reset.Pw		= __min(__max(EC20reset.Pw+updown,5),995);
						break;
						case 1: 
							EC20set.To			= __min(__max(EC20set.To+updown,100),1000);
						break;
						case 2:
							EC20reset.Period= __min(__max(EC20reset.Period+updown,2),50);
						break;
						case 3:
							break;
					}
		    
					printf("\r:EC20         %3.1lf%c,%4dus,%4dHz,",((double)EC20reset.Pw)/10,'%', EC20set.To, EC20reset.Period);
	    
					if(updown || leftright) {
						EC20energy.C=0;
						if(idx < 3) {
							lm->can.Send(Sys2Ec,(CanMsg *)&EC20set,sizeof(EC20set));
							lm->can.Send(Sys2Ec,(CanMsg *)&EC20reset,sizeof(EC20reset));
						}
					}
					
					msg.EC20Cmd.Code=Id_EC20Cmd;
					msg.EC20Cmd.Command=_NOCOMM;
					lm->pyro.enabled=true;
					switch(EC20status.Status & _STATUS_MASK) {
						case _COMPLETED						:												// standby
							sprintf(s," STNDBY");
							if(updown>0 && idx==3)
								msg.EC20Cmd.Command=_HV1_EN;
						break;
							
						case _COMPLETED + _SIM_DET:												// simmer
							sprintf(s," SIMMER");
							if(updown>0 && idx==3)
								msg.EC20Cmd.Command=_HV1_EN + _FOOT_REQ;
							if(updown<0 && idx==3)
								msg.EC20Cmd.Command=0;
						break;
						
						case _COMPLETED  + _SIM_DET + _FOOT_ACK:
							sprintf(s," LASE..");														// lasing
							if(updown<0 && idx==3)
								msg.EC20Cmd.Command=_HV1_EN;
						break;
	    
						default:
							sprintf(s," wait..");														// cakanje na ec20
							break;
						}
						printf("%s",s);
						
						if(msg.EC20Cmd.Command != _NOCOMM)
							lm->can.Send(Sys2Ec,&msg,sizeof(EC20Cmd));
	    
						char c[128];
						sprintf(c,"  %3.1lfJ,%5dW,%3.1lf'C,%3.1lf'C,%5.1lf",
																												(double)EC20energy.C/1000,
																													EC20energy.C*EC20reset.Period/1000,
																														(double)_ADC::Th2o()/100,
																															(lm->plotA-7800.0)/200.0+25.0,
																																(double)lm->plotB);
																														
						if(EC20energy.C != 0)												// ce strel, izpis energije, temperature
							printf("%s",c);
						else
							printf("%*c",strlen(c)+1,' ');
						
						for(int i=7*(8-idx)+3; i--; printf("\b"));	// mismas... poravnava kurzorja	xD
						return 0;
}
/*******************************************************************************/
/**
	* @brief	Increment
	* @param	: None
	* @retval : None
	*/
/******************************************************************************/	
void		_EC20::ECsimulator(void *v) {
_CAN 		*can = static_cast<_CAN *>(v);
				EC20energy.C=EC20energy.UI=pow((double)(EC20set.Uo * EC20reset.Pw / 1000),3)/400*EC20set.To/1000;
				can->Send(Ec2Sys,(CanMsg *)&EC20energy,sizeof(EC20energy));
}
/**
* @}
*/ 
