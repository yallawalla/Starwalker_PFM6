#ifndef				CAN_H
#define				CAN_H
#include			"stm32f2xx.h"
#include			<stdio.h>
#include			"isr.h"
#include			"ec20.h"

#ifdef	__DISCO__
#define				__CAN__						CAN1
#define				__FILT_BASE__			0
#define				CAN_GPIO					GPIOD
#define				CAN_RXPIN					0
#define				CAN_TXPIN					1
#define 			GPIO_AF_CAN 			GPIO_AF_CAN1	
#endif
#if defined   (__IOC_V1__) || defined  (__IOC_V2__)
#define				__CAN__						CAN2
#define				__FILT_BASE__			14
#define				CAN_GPIO					GPIOB
#define				CAN_RXPIN					5
#define				CAN_TXPIN					13
#define 			GPIO_AF_CAN 			GPIO_AF_CAN2	
#endif

typedef enum {
	Sys2Ioc				=0x20,
	Ioc2Sys				=0x40,
	Sys2Ec				=0x21,
	Ec2Sys				=0x41,
	Ec2Sync				=0x42,					// sync. 300u prior to laser
	Can2ComIoc		=0xB0,					// local console access req.
  Com2CanIoc		=0xB1,					// local console data, transmit only, no filter
	Com2CanEc20		=0xB3,					// ec20 console data
	Can2ComEc20		=0xBA,					// ec20 console access req. transmit only, no filter
	Footswch			=0x22,					// footswitch req. to ec20, transmit only, no filter
	SprayStatus		=0x23,					
	SprayCommand	=0x24					
} _stdid;

typedef enum {
	Id_EC20Status	=0x00,
	Id_EC20Com		=0x02,
	Id_EC20Set		=0x03,
	Id_EC20Reset	=0x14,
	Id_EC20Energy	=0x07,
	Id_EC20SyncReq=0x22,
	Id_EC20SyncAck=0x23
} _code;

typedef __packed struct {
	_code						Code;
	unsigned short	Status;
	unsigned short	Error;
} EC20status;

typedef __packed struct {
	_code						Code;
	unsigned short	Command;
} EC20com;

typedef __packed struct {
	_code						Code;
	unsigned short	Uo;
	unsigned short	To;
	unsigned char		Mode;
} EC20set;

typedef __packed struct {
	_code						Code;
	unsigned short	Period;
	unsigned short	Pw;
	unsigned char		Fo;
} EC20reset;

typedef __packed struct {
	_code						Code;
	unsigned short	C;
	unsigned short	UI;
} EC20energy;

typedef __packed struct {
	_code						Code;
} EC20SyncAck;

typedef union CanMsg {
	EC20status	EC20status;
	EC20set			EC20set;
	EC20reset		EC20reset;
	EC20energy	EC20energy;
	EC20com			EC20com;
	EC20SyncAck	EC20SyncAck;
} CanMsg;

//______________________________________________________________________________________		
//
//
//
class	_CAN {
	private:
		_buffer		*rx,*tx;
		_io				*com;
		int				debug;
	
	public:
#if defined (__DISCO__)
		_CAN	(bool=true);
#else
		_CAN	(bool=false);
#endif
		_EC20				ec20;
		void	RX_ISR(_CAN *), TX_ISR(_CAN *);
		void 	Parse(void *);
		void	Send(CanTxMsg *);
		void	Send(char *);
		void	Send(_stdid, CanMsg *, size_t dlc);
		void	Recv(char *);
};

#endif

