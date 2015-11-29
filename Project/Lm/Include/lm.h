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
#include				"adc.h"
#include				"tim.h"
#include				"dac.h"
#include				"can.h"
#include				"pyro.h"
#include				"isr.h"

#define					SW_version	10

typedef enum		{DBG_CAN_TX, DBG_CAN_RX, DBG_ERR, DBG_INFO} 																								__DEBUG__;			// debug channels
typedef enum		{PYRO, PLOT_OFFSET, PLOT_SCALE, PUMP, FAN, SPRAY, EC20, CTRL_A, CTRL_B, CTRL_C, CTRL_D} 		__SELECTED__;		// UI channels

//_____________________________________________________________________________
class	_LM {

	private:
		__SELECTED__ 	item;
		_io					*io;
		_TERM				VT100; 

		int					Decode(char *c);
		int					PlusDecode(char *c);
		int					MinusDecode(char *c);
		int					WhatDecode(char *c);
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

#ifdef	__DISCO__
	_LCD				lcd;
#endif

		__DEBUG__			debug;
		__SELECTED__	Selected(void)	{return item; }
		void 					Select(__SELECTED__);
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
