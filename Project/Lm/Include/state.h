#ifndef					STATE_H
#define					STATE_H
#include				"stm32f2xx.h"

typedef enum {
	e_Start,
	e_Timeout,
	e_Error,
	req_Ready,
	req_Standby,
	req_Active
} Event;

class	_STATE {
	private:
		bool current;
	public:
		_STATE();
		~_STATE();
	
	virtual void OnEntry(void) {}
	virtual void OnExit(void) {}
	virtual void OnIdle(void) {}
	virtual void OnEvent(Event e) {}
};


class _STANDBY : public _STATE {
	virtual void OnEntry(void) {}
	virtual void OnExit(void) {}
	virtual void OnIdle(void) {}

	virtual void OnEvent(Event e) {
		switch(e) {
			case e_Timeout:
				break;
			
			default:
			
		}
	}
};




#endif
