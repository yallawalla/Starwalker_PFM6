#ifndef					PUMP_H
#define					PUMP_H
#include				"stm32f2xx.h"
#include				"stdio.h"
#include				"adc.h"
#include				"dac.h"
#include				"tim.h"

class	_PUMP:public _ADC,_DAC {
	private:
int		idx,led;
int		to,timeout,tau;
int		fpl,fph,ftl,fth;
	public:
_PUMP();

bool	Poll(void);
int		Rpm(void);
void	Align(void);
int		Increment(int, int);
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
};

#endif
