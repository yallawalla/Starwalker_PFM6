#ifndef					EC20_H
#define					EC20_H
#include				"stm32f2xx.h"
#include				"stdio.h"
#include				"isr.h"

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
	Id_EC20Eo			=0x07,
	Id_EC20SyncReq=0x22,
	Id_EC20SyncAck=0x23
} _code;

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

void			Send2Can(_stdid, void *, size_t);

typedef __packed struct _EC20Status {
	_code						code;
	unsigned short	Status;
	unsigned short	Error;
	_EC20Status() : code(Id_EC20Status),Status(0),Error(0) {}	
	void	Send(_stdid s) { Send2Can(s,(void *)&code,sizeof(_EC20Status)); };
	} _EC20Status;

typedef __packed struct _EC20Cmd {
	_code						code;
	unsigned short	Cmd;
	_EC20Cmd() : code(Id_EC20Cmd),Cmd(_NOCOMM) {}
	void	Send(_stdid s) { Send2Can(s,(void *)&code,sizeof(_EC20Cmd)); };
} _EC20Cmd;

typedef __packed struct _EC20Set {
	_code						code;
	unsigned short	Uo;
	unsigned short	To;
	unsigned char		Mode;
	_EC20Set() : code(Id_EC20Set),Uo(420),To(200),Mode(0x02) {}
	void	Send(_stdid s) { Send2Can(s,(void *)&code,sizeof(_EC20Set)); };
} _EC20Set;

typedef __packed struct _EC20Reset {
	_code						code;
	unsigned short	Period;
	unsigned short	Pw;
	unsigned char		Fo;
	_EC20Reset() : code(Id_EC20Reset),Period(2),Pw(500),Fo(100) {}
	void	Send(_stdid s) { Send2Can(s,(void *)&code,sizeof(_EC20Reset)); };
} _EC20Reset;

typedef __packed struct _EC20Eo {
	_code						code;
	unsigned short	UI;
	unsigned short	C;
	_EC20Eo() : code(Id_EC20Eo),UI(0),C(0) {}
	void	Send(_stdid s) { Send2Can(s,(void *)&code,sizeof(_EC20Eo)); };
} _EC20Eo;

class	_EC20 {
	private:
		void *parent;
		int		timeout;
	public:
		_EC20(void *);
		~_EC20();

	_EC20Status		EC20Status;
	_EC20Cmd			EC20Cmd;
	_EC20Set			EC20Set;
	_EC20Reset		EC20Reset;
	_EC20Eo				EC20Eo;

	int			idx;
	
	int			Increment(int, int);
	void		LoadSettings(FILE *);
	void		SaveSettings(FILE *);
	void		Parse(CanTxMsg	*);
	
	static 	void	ECsimulator(void *);

	bool		Timeout(void)		{ return timeout && __time__ > timeout; }
	void		Timeout(int t)	{ t > 0 ? timeout = __time__ + t : timeout=0; }
};
#endif
