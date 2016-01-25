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
#include				"gpio.h"
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

#define					SW_version	10

typedef enum		{DBG_CAN_TX, DBG_CAN_RX, DBG_ERR, DBG_INFO} 																											_DEBUG_;			// debug channels
typedef enum		{PYRO, PILOT, PLOT_OFFSET, PLOT_SCALE, PUMP, FAN, SPRAY, EC20, CTRL_A, CTRL_B, CTRL_C, CTRL_D} 		_SELECTED_;		// UI channels

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
		int					timeout;

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
		_EC20				ec20;
		_EE					ee;
		_PILOT			pilot;

#ifdef	__DISCO__
	_LCD				lcd;
#endif

		_DEBUG_				debug;
		_SELECTED_		Selected(void)	{return item; }
		void 					Select(_SELECTED_);
		
		void 					Increment(int, int);
		
		void 					Refresh(void)		{Increment(0,0);}
		void					ChangeOffset(int);
		void					ChangeGain(int);
		bool					Parse(FILE *);
		bool					Parse(void);
		bool					Parse(int);

		static void		Poll(void *),
									Print(void *),
									Display(void *);
	};
#endif
