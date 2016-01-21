#ifndef					PILOT_H
#define					PILOT_H
#include				"stm32f2xx.h"
#include				"stdio.h"

class	_PILOT {
	private:
		int	count;

	public:
		_PILOT();
		~_PILOT();
void	Poll(void);
int		value;
void	LoadSettings(FILE *);
void	SaveSettings(FILE *);
};

#endif
