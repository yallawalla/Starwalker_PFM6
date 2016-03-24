#ifndef					LM_H
#define					LM_H

/**
	******************************************************************************
	* @file		lm.cpp
	* @author	Fotona d.d.
	* @version
	* @date		
	* @class	_LM		
	* @brief	lightmaster application class
	*
	*/

#include				"stm32f2xx.h"
#include				<stdio.h>
#include				"isr.h"
#include				"gpio.h"
#include				"term.h"
#include				"spray.h"
#include				"pump.h"
#include				"fan.h"
#include				"ec20.h"
#include				"ee.h"
#include				"adc.h"
#include				"tim.h"
#include				"dac.h"
#include				"can.h"
#include				"pyro.h"
#include				"pilot.h"
#include				"ws2812.h"

#define					SW_version	10

typedef enum		{DBG_CAN_TX, DBG_CAN_RX, DBG_ERR, DBG_INFO, DBG_CAN_COM=21, DBG_EC_SIM=22}	_DEBUG_;
typedef enum		{PYRO, PILOT, PLOT_OFFSET, PLOT_SCALE, PUMP, FAN, SPRAY, 
									EC20, CTRL_A, CTRL_B, CTRL_C, CTRL_D, REMOTE_CONSOLE, NONE} _SELECTED_;

#define	_SET_BIT(p,a)			(*(char *)(0x22000000 + ((int)&p - 0x20000000) * 32 + 4*a)) = 1
#define	_CLEAR_BIT(p,a)		(*(char *)(0x22000000 + ((int)&p - 0x20000000) * 32 + 4*a)) = 0
#define	_BIT(p,a)					(*(char *)(0x22000000 + ((int)&p - 0x20000000) * 32 + 4*a))
//_____________________________________________________________________________
class	_LM {

	private:
		_io					*io;
		_SELECTED_ 	item;
		_TERM				VT100; 

		int					Decode(char *c);
		int					DecodePlus(char *c);
		int					DecodeMinus(char *c);
		int					DecodeWhat(char *c);
		int					DecodeEq(char *c);

	public:
		_LM();
		~_LM();

		double			plotA,plotB,plotC;
		_PLOT<double> plot;	

		_SPRAY			spray;
		_CAN				can;
		_PYRO				pyro;
		_PUMP				pump;
		_FAN				fan;
		_EE					ee;
		_PILOT			pilot;
		_WS2812			ws;

#ifdef	__DISCO__
		_LCD				lcd;
#endif

		int						debug;
		void 					Increment(int, int);
		void 					Select(_SELECTED_);
		_SELECTED_		Selected(void)	{return item; }
		
		void 					Refresh(void)		{Increment(0,0);}
		bool					Parse(FILE *);
		bool					Parse(void);
		bool					Parse(int);
		void					RemoteConsole(int, int);
		
		static void		Poll(void *),
									Print(void *),
									Display(void *);
	};
#endif
