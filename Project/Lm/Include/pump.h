#ifndef					PUMP_H
#define					PUMP_H
#include				"stm32f2xx.h"
#include				"stdio.h"
#include				"stdlib.h"
#include				"adc.h"
#include				"dac.h"
#include				"tim.h"
#include				"fit.h"

class	_PUMP:public _ADC,_DAC,_TIM3 {
	private:

int		idx,led;
int		fpl,fph,ftl,fth;
_FIT	*tacho,*pressure,*current;
	
	public:
_PUMP();

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
