#ifndef					EC20_H
#define					EC20_H
#include				"stm32f2xx.h"
#include				"stdio.h"

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
	Id_EC20Cmd		=0x02,
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
} _EC20status;

typedef __packed struct {
	_code						Code;
	unsigned short	Command;
} _EC20Cmd;

typedef __packed struct {
	_code						Code;
	unsigned short	Uo;
	unsigned short	To;
	unsigned char		Mode;
} _EC20set;

typedef __packed struct {
	_code						Code;
	unsigned short	Period;
	unsigned short	Pw;
	unsigned char		Fo;
} _EC20reset;

typedef __packed struct {
	_code						Code;
	unsigned short	C;
	unsigned short	UI;
} _EC20energy;

typedef __packed struct {
	_code						Code;
} _EC20SyncAck;

typedef union CanMsg {
	_EC20status		EC20status;
	_EC20Cmd			EC20Cmd;
	_EC20set			EC20set;
	_EC20reset		EC20reset;
	_EC20energy		EC20energy;
	_EC20SyncAck	EC20SyncAck;
} CanMsg;

/* ec20 states as from status....																				*******/
#define 	_COMPLETED	0x8000
#define		_SIMGEN			0x4000
#define 	_SIM_DET		0x0001		
#define 	_TS1_IOC		0x1000
#define 	_FOOT_ACK		0x0200
#define		_STATUS_MASK				(_COMPLETED  +  _SIM_DET  +  _FOOT_ACK)

/* ec20 status commands																									*******/
#define		_SIMGEN			0x4000
#define		_HV1_EN			0x0001
#define 	_FOOT_REQ		0x0100
#define 	_NOCOMM			0xffff

class	_EC20 {
	private:
	public:
		_EC20();
		~_EC20();

	static _EC20status	EC20status;
	static _EC20Cmd			EC20Cmd;
	static _EC20set			EC20set;
	static _EC20reset		EC20reset;
	static _EC20energy	EC20energy;

	int			idx;
	
	int			Increment(int, int, void *);
	void		LoadSettings(FILE *);
	void		SaveSettings(FILE *);
	static void	ECsimulator(void *);
};
#endif
