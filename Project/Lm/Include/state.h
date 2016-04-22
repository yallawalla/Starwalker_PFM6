#ifndef					STATE_H
#define					STATE_H
#include				"stm32f2xx.h"
#include				<stdio.h>
#include				<isr.h>

typedef enum {
	req_Standby,
	req_Ready,
	req_Active,
	evt_Error,
	evt_Timeout
} Event;

class	_STATE {
	private:

	public:
	static _STATE *active;

	_STATE()		{
		if(active==NULL)
			active=this;
	};
	~_STATE()		{active=NULL;};

	virtual void OnEntry(void)		{}
	virtual void OnIdle(void)			{}
	virtual void OnEvent(Event e) {}
	virtual void OnExit(void) {
		printf("\r\n:leaving %08X\r\n:",(int)this);
	}

	_STATE	*stateChange(_STATE *s) {
		_STATE::active->OnExit();
		_STATE::active=s;
		_STATE::active->OnEntry();
		return s;
	}
};

class _STANDBY : public _STATE {
	virtual void OnEvent(Event);
	virtual void OnEntry(void) {
		printf("\r\n:entering _STANDBY\r\n:");
	}
};
	
class _READY : public _STATE {
public:
	virtual void OnEvent(Event);
	virtual void OnEntry(void) {
		printf("\r\n:entering _READY\r\n:");
	}
};

class _ACTIVE : public _STATE {
	virtual void OnIdle(void);
	virtual void OnEvent(Event);
	virtual void OnEntry(void) {
		printf("\r\n:entering _ACTIVE\r\n:");
	}
};

class _ERROR : public _STATE {
	virtual void OnEvent(Event);
		virtual void OnEntry(void) {
		printf("\r\n:entering _ERROR\r\n:");
	}
};

extern _STANDBY	_Standby;
extern _READY		_Ready;
extern _ACTIVE	_Active;
extern _ERROR		_Error;

#endif
