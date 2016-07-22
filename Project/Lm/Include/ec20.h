#ifndef					EC20_H
#define					EC20_H
#include				"stm32f2xx.h"
#include				"stdio.h"
#include				"isr.h"

typedef enum {
	Sys2Ioc				=0x20,					//
	Ioc2Sys				=0x40,					//
	Sys2Ec				=0x21,					//
	Ec2Sys				=0x41,					//
	Ec2Sync				=0x42,					// sync. message, 300u prior to laser
	Can2ComIoc		=0xB0,					// IOC local console access req.
  Com2CanIoc		=0xB1,					// IOC local console data, transmit only, no filter
	Can2ComEc20		=0xBA,					// EC20 console access req. transmit only, no filter
	Com2CanEc20		=0xB3,					// EC20 console data
	SprayStatus		=0x23,					
	SprayCommand	=0x24					
} _stdid;

typedef enum {
	Id_EC20Status	=0x00,					// status report; ec >> sys 
	Id_EC20Cmd		=0x02,					// command frame; sys >> ec
	Id_EC20Set		=0x03,					// set Uo, To, mode; sys >> ec
	Id_EC20Reset	=0x14,					// set repetition, pw, fo; sys >> ec
	Id_EC20Eo			=0x07						// energy ack; ec >> sys
} _code;

typedef enum {
	__FOOT_OFF	=0x0000f800,
	__FOOT_IDLE	=0x00003800,
	__FOOT_MID	=0x0000b800,
	__FOOT_ON		=0x0000d800,
	__FOOT_ACK	=0x0000ffff
} __FOOT;

// ec20 command bits
#define		_HV1_EN			0x0001									// simmer req
#define 	_FOOT_REQ		0x0100									// footswitch req

// ec20 bit definition as from status....
#define 	_COMPLETED	0x8000									// selftest completed after startup or reboot
#define 	_SIM_DET		0x0001									// simmer ack
#define 	_FOOT_ACK		0x0200									// footswitch ack

#define		_STATUS_MASK				(_COMPLETED  +  _SIM_DET  +  _FOOT_ACK)
void			Send2Can(_stdid, void *, size_t);

typedef __packed struct _EC20Status {
	_code						code;
	unsigned short	Status;
	unsigned short	Error;
	_EC20Status() : code(Id_EC20Status),Status(0),Error(0)			{}	
	void	Send(_stdid s) 																				{ Send2Can(s,(void *)&code,sizeof(_EC20Status)); };
	} _EC20Status;

typedef __packed struct _EC20Cmd {
	_code						code;
	unsigned short	Cmd;
	_EC20Cmd() : code(Id_EC20Cmd),Cmd(0) 												{}
	void	Send(_stdid s) 																				{ Send2Can(s,(void *)&code,sizeof(_EC20Cmd)); };
} _EC20Cmd;

typedef __packed struct _EC20Set {
	_code						code;
	unsigned short	Uo;
	unsigned short	To;
	unsigned char		Mode;
	_EC20Set() : code(Id_EC20Set),Uo(420),To(350),Mode(0x02)		{}
	void	Send(_stdid s)																				{ Send2Can(s,(void *)&code,sizeof(_EC20Set)); };
} _EC20Set;

typedef __packed struct _EC20Reset {
	_code						code;
	unsigned short	Period;
	unsigned short	Pw;
	unsigned char		Fo;
	_EC20Reset() : code(Id_EC20Reset),Period(2),Pw(300),Fo(100)	{}
	void	Send(_stdid s)																				{ Send2Can(s,(void *)&code,sizeof(_EC20Reset)); };
} _EC20Reset;

typedef __packed struct _EC20Eo {
	_code						code;
	unsigned short	UI;
	unsigned short	C;
	_EC20Eo() : code(Id_EC20Eo),UI(0),C(0) 											{}
	void	Send(_stdid s)																				{ Send2Can(s,(void *)&code,sizeof(_EC20Eo)); };
} _EC20Eo;

class	_EC20 {
	private:
		void *parent;
		int		idx,timeout;
		short	biasPw,biasT,biasF,biasN,biasNo,bias_cnt;
		_EC20Status		EC20Status;
		_EC20Cmd			EC20Cmd;
		_EC20Set			EC20Set;
		_EC20Reset		EC20Reset;
		_EC20Eo				EC20Eo;

	public:
		_EC20(void *);
		~_EC20();

	int			Increment(int, int);
	int			IncrementBias(int, int);
	int			Refresh()													{return Increment(0,0);};
	void		ReqStatus(__FOOT);
	void		LoadSettings(FILE *);
	void		SaveSettings(FILE *);
	void		Parse(CanTxMsg	*);
	
	static 	void	ECsimulator(void *);

	bool		Timeout(void)											{ return timeout && __time__ > timeout; }
	void		Timeout(int t)										{ t > 0 ? timeout = __time__ + t : timeout=0; }
};
#endif
