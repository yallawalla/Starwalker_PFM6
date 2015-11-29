#ifndef					PUMP_H
#define					PUMP_H
#include				"stm32f2xx.h"
#include				"stdio.h"
#include				"adc.h"
#include				"dac.h"
#include				"tim.h"

class	_PUMP {
	private:
int		idx,led;
int		to,timeout,tau;
int		fpl,fph,ftl,fth;
	public:
_PUMP();

bool	Poll(void);
int		Rpm(void);
int		Increment(int, int);
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
};

#endif
