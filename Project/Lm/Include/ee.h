#ifndef					EE_H
#define					EE_H

#include				"stm32f2xx.h"
#include				<stdio.h>
#include 				<stdlib.h>
#include				"isr.h"
#include				"adc.h"

#if defined   (__IOC_V2__)
#define					EE_PORT	GPIOA
#define					EE_BIT	GPIO_Pin_0
#endif

#if defined   (__DISCO__)
#define					EE_PORT	GPIOA
#define					EE_BIT	GPIO_Pin_15
#endif

class	_EE {
	private:
		_io		*io;
		int		status,nbits, temp, count, nsamples;
		bool	enabled;

	public:
		_EE();
		~_EE();
		_buffer	*buffer;
		void		ISR(_EE *);
};

typedef enum {_eeIDLE,_eeREAD,_eeWRITE,_eeRESET,_tRRT,tLOW0,tLOW1} _eestat;
#endif
