/**
  ******************************************************************************
  * @file			can.cpp
  * @author		Fotona d.d.
  * @version 
  * @date    
  * @brief		CAN initialization
  *
  */
/** @addtogroup
* @{
*/
#include	"can.h"
#include	"lm.h"
#include	"string.h"
/*******************************************************************************/
static		_CAN	*me=NULL;
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_CAN			*_CAN::Instance(void) {
					return me;	
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
_CAN::_CAN(bool loopback) {
CAN_InitTypeDef					CAN_InitStructure;
CAN_FilterInitTypeDef		CAN_FilterInitStructure;
NVIC_InitTypeDef 				NVIC_InitStructure;
				if(me==NULL) {
#ifndef __SIMULATION__					
GPIO_InitTypeDef				GPIO_InitStructure;
					GPIO_StructInit(&GPIO_InitStructure);
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
					GPIO_InitStructure.GPIO_Pin = 1<<CAN_RXPIN;
					GPIO_Init(CAN_GPIO, &GPIO_InitStructure);
					GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
					GPIO_InitStructure.GPIO_Pin = 1<<CAN_TXPIN;
					GPIO_Init(CAN_GPIO, &GPIO_InitStructure);

					GPIO_PinAFConfig(CAN_GPIO, CAN_RXPIN, GPIO_AF_CAN);
					GPIO_PinAFConfig(CAN_GPIO, CAN_TXPIN, GPIO_AF_CAN);
#endif
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);							// glej opis driverja, šmafu, treba inicializirat c
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
					CAN_StructInit(&CAN_InitStructure);
					CAN_DeInit(__CAN__);

					CAN_InitStructure.CAN_TTCM=DISABLE;
					CAN_InitStructure.CAN_ABOM=ENABLE;
					CAN_InitStructure.CAN_AWUM=DISABLE;
					CAN_InitStructure.CAN_NART=DISABLE;
					CAN_InitStructure.CAN_RFLM=DISABLE;
					
//... pomembn.. da ne zamesa mailboxov in jih oddaja po vrstnem redu vpisovanja... ni default !!!
					CAN_InitStructure.CAN_TXFP=ENABLE;	
//... prijava instance za ISR
					RX_ISR(this);
					TX_ISR(this);

					if(loopback)
						CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;
					else
						CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;

					CAN_InitStructure.CAN_SJW=CAN_SJW_4tq;
					CAN_InitStructure.CAN_BS1=CAN_BS1_10tq;
					CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;
					CAN_InitStructure.CAN_Prescaler=4;
					CAN_Init(__CAN__,&CAN_InitStructure);

					CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
					CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
					CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;
					CAN_FilterInitStructure.CAN_FilterIdLow=0;
					CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;

					CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;

	// filtri za PFM in EC
					CAN_FilterInitStructure.CAN_FilterIdHigh=Com2CanIoc<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=Com2CanEc20<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+0;
					CAN_FilterInit(&CAN_FilterInitStructure);
					CAN_FilterInitStructure.CAN_FilterIdHigh=Can2ComIoc<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=Can2ComEc20<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+1;
					CAN_FilterInit(&CAN_FilterInitStructure);

//					CAN_FilterInitStructure.CAN_FilterIdHigh=SprayCommand<<5;
//					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=SprayStatus<<5;
//					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+2;
//					CAN_FilterInit(&CAN_FilterInitStructure);

					CAN_FilterInitStructure.CAN_FilterIdHigh=Sys2Ergm<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=Ergm2Sys<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+2;
					CAN_FilterInit(&CAN_FilterInitStructure);
					
					CAN_FilterInitStructure.CAN_FilterIdHigh=Sys2Ioc<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=Sys2Ec<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+3;
					CAN_FilterInit(&CAN_FilterInitStructure);

					CAN_FilterInitStructure.CAN_FilterIdHigh=Ioc2Sys<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=Ec2Sys<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+4;
					CAN_FilterInit(&CAN_FilterInitStructure);

					CAN_FilterInitStructure.CAN_FilterIdHigh=Ec2Sync<<5;
					CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0<<5;
					CAN_FilterInitStructure.CAN_FilterNumber=__FILT_BASE__+5;
					CAN_FilterInit(&CAN_FilterInitStructure);


					NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
					NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
					NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
					NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
						
					NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
					NVIC_Init(&NVIC_InitStructure);
					NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
					NVIC_Init(&NVIC_InitStructure);

					NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
					NVIC_Init(&NVIC_InitStructure);
					NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
					NVIC_Init(&NVIC_InitStructure);
					
					CAN_ITConfig(__CAN__, CAN_IT_FMP0, ENABLE);
					com=NULL;
				}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::RX_ISR(_CAN *p) {
CanRxMsg	buf;
					if(p) {																					// klic za prijavo instance
							p->rx=_buffer_init(100*(sizeof(CanRxMsg) + sizeof(unsigned int)));
							me=p;
					} else {																				// klic iz ISR, instanca in 
						CAN_Receive(__CAN__, CAN_FIFO0, &buf);				// buffer morata bit ze formirana
						_buffer_push(me->rx,&buf,sizeof(CanRxMsg));
						_buffer_push(me->rx,(void *)&__time__,sizeof(unsigned int));
					}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::TX_ISR(_CAN *p) {
CanTxMsg	buf={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};
					if(p) {																					// klic za prijavo instance
						me=p;
						me->tx=_buffer_init(100*sizeof(CanTxMsg));
					} else {																				// klic iz ISR, instanca in buffer morata bit ze formirana
						if(_buffer_pull(me->tx,&buf,sizeof(CanTxMsg)))
							CAN_Transmit(__CAN__,&buf);									// oddaj, ce je kaj na bufferju
						else
							CAN_ITConfig(__CAN__, CAN_IT_TME, DISABLE);	// sicer zapri interrupt
					}					
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::Send(CanTxMsg *msg) {
					if(_buffer_count(tx) > 0 || (__CAN__->TSR & CAN_TSR_TME) == 0) {
						_buffer_push(tx,&msg,sizeof(msg));
					} else
						CAN_Transmit(__CAN__,msg);
					CAN_ITConfig(__CAN__, CAN_IT_TME, ENABLE);		
//________ debug print__________________________________________________________________
					if(_BIT(_LM::debug, DBG_CAN_TX)) {
//						_io *temp=_stdio(lm->io);
						printf("\r\n:%04d>%02X ",__time__ % 10000,msg->StdId);
						for(int i=0;i<msg->DLC;++i)
							printf(" %02X",msg->Data[i]);
						printf("\r\n:");
//						_stdio(temp);
					}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::Send(char *msg) {	
int				n;
CanTxMsg	buf={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};
					sscanf(msg,"%02X",&buf.StdId);	
					++msg,++msg;
					do {
						for(n=0; *msg && n<16; ++n,++n,++msg,++msg)
							sscanf(msg,"%02X",(unsigned int *)&buf.Data[n/2]);
						buf.DLC=n/2;
						Send(&buf);
					} while(*msg);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::Parse(void *v) {	

unsigned	int tstamp=0;
CanTxMsg	msg={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};		
_LM				*lm = (_LM *)v;
//
//________ flushing com buffer/not echoed if debug_________ 
					if(com &&  tx->size - _buffer_count(tx) > sizeof(CanTxMsg)) {
						msg.StdId=Com2CanIoc;
						msg.DLC=_buffer_pull(com->tx,msg.Data,sizeof(msg.Data));
						if(msg.DLC)
							Send(&msg);
					}
//______________________________________________________________________________________					
					if(_buffer_count(rx)  && _buffer_pull(rx,&msg,sizeof(CanTxMsg)) && _buffer_pull(rx,&tstamp,sizeof(unsigned int))) {
//
//________ debug print__________________________________________________________________
						if(_BIT(_LM::debug, DBG_CAN_RX)) {
//							_io *temp=_stdio(lm->io);
							printf("\r\n:%04d<%02X ",__time__ % 10000,msg.StdId);
							for(int i=0;i<msg.DLC;++i)
								printf(" %02X",msg.Data[i]);
							printf("\r\n:");
//							_stdio(temp);
						}
//______________________________________________________________________________________
						switch(msg.StdId) {
//______________________________________________________________________________________							
							case Com2CanEc20: 
							case Com2CanIoc: 
							{
								if(lm->Selected() == REMOTE_CONSOLE)
									for(int i=0; i<msg.DLC; ++i)
										putchar(msg.Data[i]);
							}
							break;
//______________________________________________________________________________________
							case Can2ComIoc:
								if(msg.DLC) {
									if(com == NULL) {
										com=_io_init(128,128);
										_thread_add((void *)ParseCom,com,(char *)"ParseCom CAN",0);	
									}
									_buffer_push(com->rx,msg.Data,msg.DLC);
								} else {
									_thread_remove((void *)ParseCom,com);	
									com=_io_close(com);
								}
								break;
//______________________________________________________________________________________
//							case SprayCommand:
//								if(msg.DLC) {
//									lm->spray.AirLevel=msg.Data[0];
//									lm->spray.WaterLevel=msg.Data[1];
//								} else {
//									char	c[64];
//									sprintf(c,">%02X%02X%02X",	SprayCommand,
//																								lm->spray.AirLevel,
//																									lm->spray.WaterLevel);
//									Send(c);
//								}
//								break;
////______________________________________________________________________________________
//							case SprayStatus:
//								char	c[64];
//								sprintf(c,">%02X%02X%02X",	SprayStatus,
//																							_ADC::Th2o()/100,
//																								_ADC::Status());
//								Send(c);
//								break;
//______________________________________________________________________________________
							case Sys2Ec:
							case Ec2Sys:								
							case Ergm2Sys: 																						// energometer messages
								lm->ec20.Parse(&msg);
								break;
//______________________________________________________________________________________					
							default:
							break;
						}
					}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void			_CAN::Recv(char *msg) {	
int				n;
CanRxMsg	rxbuf={0,0,CAN_ID_STD,CAN_RTR_DATA,0,0,0,0,0,0,0,0,0};
					sscanf(msg,"%02X",&rxbuf.StdId);	
					++msg,++msg;
					do {
						for(n=0; *msg && n<16; ++n,++n,++msg,++msg)
							sscanf(msg,"%02X",(unsigned int *)&rxbuf.Data[n/2]);
						rxbuf.DLC=n/2;
						_buffer_push(rx,&rxbuf,sizeof(rxbuf));
						_buffer_push(rx,(void *)&__time__,sizeof(unsigned int));
					} while(*msg);
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
extern 		"C" {
void 			CAN1_TX_IRQHandler() {
					me->TX_ISR(NULL);
}
void 			CAN2_TX_IRQHandler() {
					me->TX_ISR(NULL);
}
void 			CAN1_RX0_IRQHandler() {
					me->RX_ISR(NULL);
}
void 			CAN2_RX0_IRQHandler() {
					me->RX_ISR(NULL);
}
void 			CAN1_SCE_IRQHandler(void) {
					CAN1->MSR |= 0x0004;
					}
void 			CAN2_SCE_IRQHandler(void) {
					CAN2->MSR |= 0x0004;
					}
}
/**
* @}
*/


