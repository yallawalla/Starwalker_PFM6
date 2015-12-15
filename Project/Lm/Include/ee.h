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
#define					EE_BIT	GPIO_Pin_13
#endif

#define	_tRD		4
#define _tMRS		5
#define _tRCV		6
#define _tRESET	500
#define _tRRT		10
#define _tDDR		2

class	_EE {
	private:
		_io		*io;
		int		nbits, temp, phase;
		enum	{_IDLE,_WRITE,_RESET} status;

	public:
		_EE();
		~_EE();
		void			ISR(_EE *);
		void			Exchg(char *);
		uint64_t	GetSerial(void);	
};

#endif
