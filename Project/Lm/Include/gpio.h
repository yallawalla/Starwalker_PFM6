#ifndef					GPIO_H
#define					GPIO_H
#include				"stm32f2xx.h"

class	_GPIO { 
	private:
		int	key,timeout;
	public:
		_GPIO(void);
		int	Poll(void);
};
#endif
