#ifndef			FAN_H
#define			FAN_H
#include		"stm32f2xx.h"
#include		"stdio.h"
#include		"adc.h"
#include		"dac.h"
#include		"tim.h"
#include		"fit.h"
#include		"isr.h"

class	_FAN:_ADC,_TIM3 {
	private:
int		idx,led;
int		fpl, fph, ftl, fth;
_FIT	*tacho;
	
	public:
		_FAN();

error	Poll(void);
int		Rpm(void);
int		Increment(int, int);
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
bool	Align(void);
void	LoadLimits(FILE *);
void	SaveLimits(FILE *);
bool	Test(void);

};

#endif
