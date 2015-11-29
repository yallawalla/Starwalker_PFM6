#ifndef					EC20_H
#define					EC20_H
#include				"stm32f2xx.h"
#include				"stdio.h"

class	_EC20 {
	private:
	public:
		_EC20();
		~_EC20();

	int repeat,
			pw,
			width,
			Uo,
			Fo,
			mode,
			status,error,E,
			idx;
	
int		Increment(int, int, void *);
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
};
#endif
