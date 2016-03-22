#ifndef			FAN_H
#define			FAN_H
#include		"stm32f2xx.h"
#include		"stdio.h"
#include		"adc.h"
#include		"dac.h"
#include		"tim.h"
#include		"isr.h"

class	_FAN:_ADC {
	private:
int		idx,led;
int		to,timeout,tau;
int		fpl, fph, ftl, fth;
	public:
_FAN();

bool	Poll(void);
int		Rpm(void);
int		Increment(int, int);
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
};

#endif
